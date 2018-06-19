#include "tcg2llvm.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "qemu/osdep.h"
#include "target/s390x/cpu.h"

#include "exec/helper-head.h"
#include "exec/helper-proto.h"
#include "libtcg.h"
#include "tcg/tcg.h"

static const char llvm_unnamed[] = "";

static LLVMTypeRef llvm_type(TCGType type)
{
    switch (type)
    {
    case TCG_TYPE_I32:
        return LLVMInt32Type();
    case TCG_TYPE_I64:
        return LLVMInt64Type();
    default:
        fprintf(stderr, "Unsupported type: %d\n", (int)type);
        abort();
    }
}

static LLVMIntPredicate llvm_int_predicate(TCGCond cond)
{
    switch (cond) {
    case TCG_COND_EQ:
        return LLVMIntEQ;
    case TCG_COND_NE:
        return LLVMIntNE;
    case TCG_COND_LT:
        return LLVMIntSLT;
    case TCG_COND_GE:
        return LLVMIntSGE;
    case TCG_COND_LE:
        return LLVMIntSLE;
    case TCG_COND_GT:
        return LLVMIntSGT;
    case TCG_COND_LTU:
        return LLVMIntULT;
    case TCG_COND_GEU:
        return LLVMIntUGE;
    case TCG_COND_LEU:
        return LLVMIntULE;
    case TCG_COND_GTU:
        return LLVMIntUGT;
    default:
        fprintf(stderr, "Unsupported cond: %d\n", (int)cond);
        abort();
    }
}

void llvm_init(struct llvm *llvm, const char *module_id)
{
    LLVMValueRef llvm_env_offsets[2] = {
        LLVMConstInt(LLVMInt32Type(), 0, true),
        LLVMConstInt(LLVMInt32Type(), ENV_OFFSET, true),
    };

    llvm->module = LLVMModuleCreateWithName(module_id);
    llvm->memory = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), RAM_SIZE), "memory");
    llvm->cpu = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), sizeof(struct S390CPU)), "cpu");
    llvm->cpu_env = LLVMBuildGEP(llvm->builder, llvm->cpu, llvm_env_offsets, ARRAY_SIZE(llvm_env_offsets), "env");
    llvm->builder = LLVMCreateBuilder();
    memset(&llvm->labels, 0, sizeof(llvm->labels));
    memset(&llvm->locals, 0, sizeof(llvm->locals));
}

#define easy_snprintf(str, format, ...) str[snprintf(str, sizeof(str) - 1, format, __VA_ARGS__)] = 0

static LLVMBasicBlockRef llvm_bb_for_label(struct llvm *llvm, LLVMValueRef llvm_function, struct TCGLabel *label)
{
    if (label->id >= MAX_LABELS) {
        fprintf(stderr, "Label id (%d) > MAX_LABELS (%d)\n", (int)label->id, MAX_LABELS);
        abort();
    }
    if (!llvm->labels[label->id]) {
        char bb_name[32];

        easy_snprintf(bb_name, ".L%d", (int)label->id);
        llvm->labels[label->id] = LLVMAppendBasicBlock(llvm_function, bb_name);
    }
    return llvm->labels[label->id];
}

static LLVMValueRef llvm_var(struct llvm *llvm, struct TCGContext *s, TCGArg var);

static LLVMValueRef llvm_global_var(struct llvm *llvm, struct TCGContext *s, TCGTemp *temp)
{
    LLVMValueRef llvm_base_ptr;
    LLVMValueRef llvm_offsets[1];
    LLVMValueRef llvm_raw_ptr;

    if (!temp->mem_base) {
        fprintf(stderr, "Unsupported temp: %s\n", temp->name);
        abort();
    }
    llvm_base_ptr = llvm_var(llvm, s, temp_arg(temp->mem_base));
    llvm_offsets[0] = LLVMConstInt(LLVMInt32Type(), temp->mem_offset, true);
    llvm_raw_ptr = LLVMBuildGEP(llvm->builder, llvm_base_ptr, llvm_offsets, ARRAY_SIZE(llvm_offsets), llvm_unnamed);
    return LLVMBuildBitCast(llvm->builder, llvm_raw_ptr, LLVMPointerType(llvm_type(temp->type), 0), temp->name);
}

static LLVMValueRef llvm_local_var(struct llvm *llvm, struct TCGContext *s, TCGTemp *temp)
{
    size_t idx = temp_idx(temp) - s->nb_globals;

    if (idx >= MAX_LOCALS) {
        fprintf(stderr, "Temp idx (%d) > MAX_LOCALS (%d)\n", (int)idx, MAX_LOCALS);
        abort();
    }
    if (!llvm->locals[idx]) {
        const char *prefix = temp->temp_local ? "loc" : "tmp";
        char name[32];

        easy_snprintf(name, "%s%d", prefix, (int)idx);
        llvm->locals[idx] = LLVMBuildAlloca(llvm->builder, llvm_type(temp->type), name);
    }
    return llvm->locals[idx];
}

static LLVMValueRef llvm_var(struct llvm *llvm, struct TCGContext *s, TCGArg var)
{
    TCGTemp *temp = arg_temp(var);

    if (temp == tcgv_i32_temp((TCGv_i32)cpu_env))
        return llvm->cpu_env;

    if (temp->temp_global)
        return llvm_global_var(llvm, s, temp);
    else
        return llvm_local_var(llvm, s, temp);
}

static LLVMValueRef llvm_base_offset(struct llvm *llvm, struct TCGContext *s, TCGArg base, TCGArg offset)
{
    LLVMValueRef llvm_base = llvm_var(llvm, s, base);
    LLVMValueRef llvm_offsets[] = {
        LLVMConstInt(LLVMInt32Type(), (int32_t)offset, true),
    };

    return LLVMBuildGEP(llvm->builder, llvm_base, llvm_offsets, ARRAY_SIZE(llvm_offsets), llvm_unnamed);
}

void llvm_convert_tb(struct llvm *llvm, struct TCGContext *s, uint64_t pc)
{
    char name[32];
    LLVMValueRef llvm_function;
    LLVMBasicBlockRef llvm_bb;
    struct TCGOp *op;

    easy_snprintf(name, "i_%"PRIx64, pc);
    llvm_function = LLVMAddFunction(llvm->module, name, LLVMFunctionType(LLVMInt64Type(), NULL, 0, false));
    llvm_bb = LLVMAppendBasicBlock(llvm_function, llvm_unnamed);
    LLVMPositionBuilderAtEnd(llvm->builder, llvm_bb);

    QTAILQ_FOREACH (op, &s->ops, link) {
        const TCGOpDef *def = &tcg_op_defs[op->opc];

        switch (op->opc) {
        case INDEX_op_movi_i32:
        case INDEX_op_movi_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMTypeRef type = op->opc == INDEX_op_movi_i32 ? LLVMInt32Type() : LLVMInt64Type();
            LLVMValueRef t1 = LLVMConstInt(type, (uint32_t)op->args[1], false);

            LLVMBuildStore(llvm->builder, t1, t0);
            break;
        }
        case INDEX_op_ld_i32: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_base_offset(llvm, s, op->args[1], op->args[2]);
            LLVMValueRef t1p = LLVMBuildBitCast(llvm->builder, t1, LLVMPointerType(LLVMInt32Type(), 0), llvm_unnamed);
            LLVMValueRef t1v = LLVMBuildLoad(llvm->builder, t1p, llvm_unnamed);

            LLVMBuildStore(llvm->builder, t1v, t0);
            break;
        }
        case INDEX_op_brcond_i32: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t0v = LLVMBuildLoad(llvm->builder, t0, llvm_unnamed);
            LLVMValueRef t1 = llvm_var(llvm, s, op->args[1]);
            LLVMValueRef t1v = LLVMBuildLoad(llvm->builder, t1, llvm_unnamed);
            TCGCond cond = (TCGCond)op->args[2];
            TCGLabel *label = arg_label(op->args[3]);
            LLVMBasicBlockRef llvm_then_bb = llvm_bb_for_label(llvm, llvm_function, label);
            LLVMBasicBlockRef llvm_else_bb = LLVMAppendBasicBlock(llvm_function, llvm_unnamed);
            LLVMValueRef llvm_cond = LLVMBuildICmp(llvm->builder, llvm_int_predicate(cond), t0v, t1v, llvm_unnamed);

            LLVMBuildCondBr(llvm->builder, llvm_cond, llvm_then_bb, llvm_else_bb);
            llvm_bb = llvm_else_bb;
            LLVMPositionBuilderAtEnd(llvm->builder, llvm_bb);
            break;
        }
        case INDEX_op_set_label: {
            TCGLabel *label = arg_label(op->args[0]);
            LLVMBasicBlockRef llvm_next_bb = llvm_bb_for_label(llvm, llvm_function, label);

            if (llvm_bb)
                LLVMBuildBr(llvm->builder, llvm_next_bb);
            llvm_bb = llvm_next_bb;
            LLVMPositionBuilderAtEnd(llvm->builder, llvm_bb);
            break;
        }
        case INDEX_op_insn_start:
            // Ignore.
            break;
        case INDEX_op_qemu_ld_i64:
            // TODO
            break;
        case INDEX_op_call: {
            void *ptr = (void *)op->args[TCGOP_CALLO(op) + TCGOP_CALLI(op)];

            if (ptr == HELPER(lctlg)) {
                // Ignore - assume control registers are irrelevant.
            } else {
                fprintf(stderr, "Unsupported ptr: %p\n", ptr);
                abort();
            }
            break;
        }
        case INDEX_op_exit_tb: {
            int64_t val = (int64_t)op->args[0];

            LLVMBuildRet(llvm->builder, LLVMConstInt(LLVMInt64Type(), val, false));
            llvm_bb = NULL;
            break;
        }
        case INDEX_op_add_i32:
        case INDEX_op_add_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var(llvm, s, op->args[1]);
            LLVMValueRef t1v = LLVMBuildLoad(llvm->builder, t1, llvm_unnamed);
            LLVMValueRef t2 = llvm_var(llvm, s, op->args[1]);
            LLVMValueRef t2v = LLVMBuildLoad(llvm->builder, t2, llvm_unnamed);
            LLVMValueRef sum = LLVMBuildAdd(llvm->builder, t1v, t2v, llvm_unnamed);

            LLVMBuildStore(llvm->builder, sum, t0);
            break;
        }
        case INDEX_op_mov_i32:
        case INDEX_op_mov_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var(llvm, s, op->args[1]);
            LLVMValueRef t1v = LLVMBuildLoad(llvm->builder, t1, llvm_unnamed);

            LLVMBuildStore(llvm->builder, t1v, t0);
            break;
        }
        default:
            fprintf(stderr, "Unsupported op: %s\n", def->name);
            abort();
        }
    }
}
