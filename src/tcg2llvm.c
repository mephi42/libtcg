#include "tcg2llvm.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "qemu/osdep.h"
#include "target/s390x/cpu.h"

#include "hw/loader.h"
#include "libtcg.h"
#include "llvm-core-extras.h"
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

static LLVMTypeRef llvm_memop_type(TCGMemOp memop)
{
    switch (memop & MO_SIZE)
    {
    case MO_8:
        return LLVMInt8Type();
    case MO_16:
        return LLVMInt16Type();
    case MO_32:
        return LLVMInt32Type();
    case MO_64:
        return LLVMInt64Type();
    default:
        fprintf(stderr, "Unsupported memop: 0x%x\n", (int)memop);
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

static LLVMValueRef llvm_pc(struct llvm *llvm, struct CPUState *cpu)
{
    LLVMValueRef pc = LLVMBuildLoad(llvm->builder, llvm->pc, "pc");

    if (S390_CPU(cpu)->env.psw.mask & PSW_MASK_32)
        pc = LLVMBuildAnd(llvm->builder, pc, LLVMConstInt(LLVMTypeOf(pc), 0x7fffffff, false), llvm_unnamed);
    return pc;
}

static void llvm_init_dispatch(struct llvm *llvm, struct CPUState *cpu)
{
    LLVMTypeRef signature = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
    LLVMBasicBlockRef bb;
    LLVMBasicBlockRef not_found;

    llvm->dispatch = LLVMAddFunction(llvm->module, "dispatch", signature);
    bb = LLVMAppendBasicBlock(llvm->dispatch, "entry");
    not_found = LLVMAppendBasicBlock(llvm->dispatch, "not_found");

    LLVMPositionBuilderAtEnd(llvm->builder, not_found);
    LLVMBuildTrap(llvm->builder);
    LLVMBuildUnreachable(llvm->builder);

    LLVMPositionBuilderAtEnd(llvm->builder, bb);
    llvm->switch_pc = LLVMBuildSwitch(llvm->builder, llvm_pc(llvm, cpu), not_found, 10);
}

static void llvm_init_disasm(struct llvm *llvm, struct CPUState *cpu)
{
    LLVMTypeRef return_type = LLVMPointerType(LLVMInt8Type(), 0);
    LLVMTypeRef signature = LLVMFunctionType(return_type, NULL, 0, false);
    LLVMBasicBlockRef bb;
    LLVMBasicBlockRef not_found;

    llvm->disasm = LLVMAddFunction(llvm->module, "disasm", signature);
    bb = LLVMAppendBasicBlock(llvm->disasm, "entry");
    not_found = LLVMAppendBasicBlock(llvm->disasm, "not_found");

    LLVMPositionBuilderAtEnd(llvm->builder, not_found);
    LLVMBuildRet(llvm->builder, LLVMConstNull(return_type));

    LLVMPositionBuilderAtEnd(llvm->builder, bb);
    llvm->disasm_switch = LLVMBuildSwitch(llvm->builder, llvm_pc(llvm, cpu), not_found, 10);
}

static void llvm_init_globals(struct llvm *llvm, struct CPUState *cpu)
{
    struct S390CPU *s390_cpu = S390_CPU(cpu);
    hwaddr image_size = llvm->image_size;
    void *image = cpu_physical_memory_map(0, &image_size, false);
    LLVMValueRef memory_init = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), llvm->image_size), "memory_init");
    LLVMValueRef memory_init_size = LLVMAddGlobal(llvm->module, LLVMInt32Type(), "memory_init_size");
    LLVMValueRef cpu_model = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), sizeof(struct S390CPUModel)), "cpu_model");
    LLVMValueRef cpu_def = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), sizeof(struct S390CPUDef)), "cpu_def");

    LLVMSetInitializer(llvm->cpu, LLVMConstString((const char *)s390_cpu, sizeof(*s390_cpu), true));
    LLVMSetInitializer(memory_init, LLVMConstString(image, image_size, true));
    LLVMSetInitializer(memory_init_size, LLVMConstInt(LLVMInt32Type(), llvm->image_size, false));
    LLVMSetInitializer(cpu_model, LLVMConstString((const char *)s390_cpu->model, sizeof(*s390_cpu->model), true));
    LLVMSetInitializer(cpu_def, LLVMConstString((const char *)s390_cpu->model->def, sizeof(*s390_cpu->model->def), true));
}

void llvm_init(struct llvm *llvm, struct CPUState *cpu, const char *path)
{
    LLVMTypeRef memory_type = LLVMArrayType(LLVMInt8Type(), RAM_SIZE);
    LLVMValueRef llvm_env_offsets[] = {
        LLVMConstInt(LLVMInt32Type(), 0, false),
        LLVMConstInt(LLVMInt32Type(), ENV_OFFSET, false),
    };
    LLVMValueRef llvm_pc_offsets[] = {
        LLVMConstInt(LLVMInt32Type(), offsetof(CPUS390XState, psw.addr), false),
    };

    llvm->module = LLVMModuleCreateWithName(path);
    llvm->memory = LLVMAddGlobal(llvm->module, memory_type, "memory");
    LLVMSetInitializer(llvm->memory, LLVMConstNull(memory_type));
    llvm->cpu = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), sizeof(struct S390CPU)), "cpu");
    llvm->cpu_env = LLVMConstGEP(llvm->cpu, llvm_env_offsets, ARRAY_SIZE(llvm_env_offsets));
    llvm->pc = LLVMConstBitCast(
                LLVMConstGEP(llvm->cpu_env, llvm_pc_offsets, ARRAY_SIZE(llvm_pc_offsets)),
                LLVMPointerType(LLVMInt64Type(), 0));
    llvm->builder = LLVMCreateBuilder();
    llvm->image_size = get_image_size(path);
    llvm_init_globals(llvm, cpu);
    llvm_init_dispatch(llvm, cpu);
    llvm_init_disasm(llvm, cpu);
}

static void llvm_register_pc(struct llvm *llvm, uint64_t pc, LLVMValueRef function, LLVMValueRef disasm)
{
    LLVMBasicBlockRef bb;
    LLVMValueRef call;
    LLVMValueRef disasm_indices[] = {
        LLVMConstInt(LLVMInt32Type(), 0, false),
        LLVMConstInt(LLVMInt32Type(), 0, false),
    };
    LLVMValueRef disasm_ptr;
    LLVMValueRef llvm_pc = LLVMConstInt(LLVMInt64Type(), pc, false);

    bb = LLVMAppendBasicBlock(llvm->dispatch, llvm_unnamed);
    LLVMPositionBuilderAtEnd(llvm->builder, bb);
    call = LLVMBuildCall(llvm->builder, function, NULL, 0, llvm_unnamed);
    LLVMSetTailCall(call, true);
    LLVMBuildRetVoid(llvm->builder);
    LLVMAddCase(llvm->switch_pc, llvm_pc, bb);

    bb = LLVMAppendBasicBlock(llvm->disasm, llvm_unnamed);
    LLVMPositionBuilderAtEnd(llvm->builder, bb);
    disasm_ptr = LLVMBuildGEP(llvm->builder, disasm, disasm_indices, ARRAY_SIZE(disasm_indices), llvm_unnamed);
    LLVMBuildRet(llvm->builder, disasm_ptr);
    LLVMAddCase(llvm->disasm_switch, llvm_pc, bb);
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

static LLVMValueRef llvm_base_offset(struct llvm *llvm, struct TCGContext *s, TCGArg base, TCGArg offset, LLVMTypeRef type);

static LLVMValueRef llvm_global_var(struct llvm *llvm, struct TCGContext *s, TCGTemp *temp)
{
    if (!temp->mem_base) {
        fprintf(stderr, "Unsupported temp: %s\n", temp->name);
        abort();
    }
    return llvm_base_offset(llvm, s, temp_arg(temp->mem_base), temp->mem_offset, llvm_type(temp->type));
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

    if (temp->temp_global)
        return llvm_global_var(llvm, s, temp);
    else
        return llvm_local_var(llvm, s, temp);
}

static LLVMValueRef llvm_var_value(struct llvm *llvm, struct TCGContext *s, TCGArg var)
{
    if (arg_temp(var) == tcgv_i32_temp((TCGv_i32)cpu_env))
        return llvm->cpu_env;
    return LLVMBuildLoad(llvm->builder, llvm_var(llvm, s, var), llvm_unnamed);
}

static LLVMValueRef llvm_base_offset(struct llvm *llvm, struct TCGContext *s, TCGArg base, TCGArg offset, LLVMTypeRef type)
{
    LLVMValueRef llvm_offset = LLVMConstInt(LLVMInt64Type(), (int32_t)offset, false);
    LLVMValueRef i8_ptr;

    if (arg_temp(base) == tcgv_i32_temp((TCGv_i32)cpu_env)) {
        LLVMValueRef llvm_offsets[] = {
            llvm_offset,
        };
        i8_ptr = LLVMBuildGEP(llvm->builder, llvm->cpu_env, llvm_offsets, ARRAY_SIZE(llvm_offsets), llvm_unnamed);
    } else {
        LLVMValueRef llvm_base = llvm_var_value(llvm, s, base);
        LLVMValueRef llvm_addr = LLVMBuildAdd(llvm->builder, llvm_base, llvm_offset, llvm_unnamed);
        LLVMValueRef llvm_offsets[] = {
            LLVMConstInt(LLVMInt32Type(), 0, false),
            llvm_addr,
        };
        i8_ptr = LLVMBuildGEP(llvm->builder, llvm->memory, llvm_offsets, ARRAY_SIZE(llvm_offsets), llvm_unnamed);
    }
    return LLVMBuildBitCast(llvm->builder, i8_ptr, LLVMPointerType(type, 0), llvm_unnamed);
}

typedef LLVMValueRef (*llvm_un_op_f)(LLVMBuilderRef, LLVMValueRef, const char *);

static LLVMValueRef llvm_un_op(struct llvm *llvm, struct TCGContext *s, TCGArg dst, llvm_un_op_f f, TCGArg op)
{
    LLVMValueRef llvm_dst = llvm_var(llvm, s, dst);
    LLVMValueRef llvm_op = llvm_var_value(llvm, s, op);
    LLVMValueRef result = f(llvm->builder, llvm_op, llvm_unnamed);

    return LLVMBuildStore(llvm->builder, result, llvm_dst);
}

typedef LLVMValueRef (*llvm_bin_op_f)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, const char *);

static LLVMValueRef llvm_bin_op(struct llvm *llvm, struct TCGContext *s, TCGArg dst, TCGArg op1, llvm_bin_op_f f, TCGArg op2)
{
    LLVMValueRef llvm_dst = llvm_var(llvm, s, dst);
    LLVMValueRef llvm_op1 = llvm_var_value(llvm, s, op1);
    LLVMValueRef llvm_op2 = llvm_var_value(llvm, s, op2);
    LLVMValueRef result = f(llvm->builder, llvm_op1, llvm_op2, llvm_unnamed);

    return LLVMBuildStore(llvm->builder, result, llvm_dst);
}

typedef LLVMValueRef (*llvm_cast_op_f)(LLVMBuilderRef, LLVMValueRef, LLVMTypeRef, const char *);

static LLVMValueRef llvm_cast_op(struct llvm *llvm, struct TCGContext *s, TCGArg dst, llvm_cast_op_f f, TCGArg op)
{
    LLVMValueRef llvm_dst = llvm_var(llvm, s, dst);
    LLVMTypeRef type = LLVMGetElementType(LLVMTypeOf(llvm_dst));
    LLVMValueRef llvm_op = llvm_var_value(llvm, s, op);
    LLVMValueRef result = f(llvm->builder, llvm_op, type, llvm_unnamed);

    return LLVMBuildStore(llvm->builder, result, llvm_dst);
}

static LLVMValueRef llvm_memop_bswap(struct llvm *llvm, LLVMValueRef v, TCGMemOp memop)
{
    return (memop & MO_BSWAP) ? LLVMBuildBSwap(llvm->builder, v, llvm_unnamed) : v;
}

static LLVMValueRef llvm_ext_ld(struct llvm *llvm, struct TCGContext *s, TCGArg dst, TCGArg base, TCGArg offset, LLVMTypeRef type, llvm_cast_op_f f)
{
    LLVMValueRef llvm_dst = llvm_var(llvm, s, dst);
    LLVMValueRef llvm_op = llvm_base_offset(llvm, s, base, offset, type);
    LLVMValueRef llvm_op_val = LLVMBuildLoad(llvm->builder, llvm_op, llvm_unnamed);
    LLVMValueRef result = f(llvm->builder, llvm_op_val, LLVMGetElementType(LLVMTypeOf(llvm_dst)), llvm_unnamed);

    return LLVMBuildStore(llvm->builder, result, llvm_dst);
}

// tcg.c:656
typedef struct TCGHelperInfo {
    void *func;
    const char *name;
    unsigned flags;
    unsigned sizemask;
} TCGHelperInfo;

// tcg.c:668
extern GHashTable *helper_table;

LLVMValueRef llvm_convert_tb(struct llvm *llvm, struct TCGContext *s, uint64_t pc, const char *disasm)
{
    char name[32];
    LLVMValueRef llvm_function;
    LLVMBasicBlockRef llvm_bb;
    struct TCGOp *op;
    bool insn_started = false;
    size_t disasm_len = strlen(disasm);
    LLVMValueRef llvm_disasm;

    memset(&llvm->labels, 0, sizeof(llvm->labels));
    memset(&llvm->locals, 0, sizeof(llvm->locals));

    easy_snprintf(name, "disasm_0x%"PRIx64, pc);
    llvm_disasm = LLVMAddGlobal(llvm->module, LLVMArrayType(LLVMInt8Type(), disasm_len + 1), name);
    LLVMSetLinkage(llvm_disasm, LLVMInternalLinkage);
    LLVMSetInitializer(llvm_disasm, LLVMConstString(disasm, disasm_len + 1, true));

    easy_snprintf(name, "pc_0x%"PRIx64, pc);
    llvm_function = LLVMAddFunction(llvm->module, name, LLVMFunctionType(LLVMVoidType(), NULL, 0, false));
    LLVMSetLinkage(llvm_function, LLVMInternalLinkage);
    llvm_bb = LLVMAppendBasicBlock(llvm_function, llvm_unnamed);
    LLVMPositionBuilderAtEnd(llvm->builder, llvm_bb);

    QTAILQ_FOREACH (op, &s->ops, link) {
        const TCGOpDef *def = &tcg_op_defs[op->opc];

        if (!insn_started) {
            if (op->opc == INDEX_op_insn_start)
                insn_started = true;
            continue;  // Ignore icount (gen-icount.h:21)
        }
        switch (op->opc) {
        case INDEX_op_movi_i32:
        case INDEX_op_movi_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMTypeRef type = LLVMGetElementType(LLVMTypeOf(t0));
            LLVMValueRef t1 = LLVMConstInt(type, (uint64_t)op->args[1], false);

            LLVMBuildStore(llvm->builder, t1, t0);
            break;
        }
        case INDEX_op_ld_i32:
        case INDEX_op_ld_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_base_offset(llvm, s, op->args[1], op->args[2], LLVMGetElementType(LLVMTypeOf(t0)));
            LLVMValueRef t1v = LLVMBuildLoad(llvm->builder, t1, llvm_unnamed);

            LLVMBuildStore(llvm->builder, t1v, t0);
            break;
        }
        case INDEX_op_ld8s_i32:
        case INDEX_op_ld8s_i64:
            llvm_ext_ld(llvm, s, op->args[0], op->args[1], op->args[2], LLVMInt8Type(), &LLVMBuildSExt);
            break;
        case INDEX_op_ld8u_i32:
        case INDEX_op_ld8u_i64:
            llvm_ext_ld(llvm, s, op->args[0], op->args[1], op->args[2], LLVMInt8Type(), &LLVMBuildZExt);
            break;
        case INDEX_op_ld16s_i32:
        case INDEX_op_ld16s_i64:
            llvm_ext_ld(llvm, s, op->args[0], op->args[1], op->args[2], LLVMInt16Type(), &LLVMBuildSExt);
            break;
        case INDEX_op_ld16u_i32:
        case INDEX_op_ld16u_i64:
            llvm_ext_ld(llvm, s, op->args[0], op->args[1], op->args[2], LLVMInt16Type(), &LLVMBuildZExt);
            break;
        case INDEX_op_ld32s_i64:
            llvm_ext_ld(llvm, s, op->args[0], op->args[1], op->args[2], LLVMInt32Type(), &LLVMBuildSExt);
            break;
        case INDEX_op_ld32u_i64:
            llvm_ext_ld(llvm, s, op->args[0], op->args[1], op->args[2], LLVMInt32Type(), &LLVMBuildZExt);
            break;
        case INDEX_op_st_i32:
        case INDEX_op_st_i64: {
            LLVMValueRef t0 = llvm_var_value(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_base_offset(llvm, s, op->args[1], op->args[2], LLVMTypeOf(t0));

            LLVMBuildStore(llvm->builder, t0, t1);
            break;
        }
        case INDEX_op_st8_i32:
        case INDEX_op_st8_i64:
        case INDEX_op_st16_i32:
        case INDEX_op_st16_i64:
        case INDEX_op_st32_i64: {
            LLVMValueRef t0 = llvm_var_value(llvm, s, op->args[0]);
            LLVMTypeRef type = (op->opc == INDEX_op_st8_i32 || op->opc == INDEX_op_st8_i64) ?
                        LLVMInt8Type() :
                        ((op ->opc == INDEX_op_st16_i32 || op->opc == INDEX_op_st16_i64) ?
                             LLVMInt16Type() :
                             LLVMInt32Type());
            LLVMValueRef val = LLVMBuildTrunc(llvm->builder, t0, type, llvm_unnamed);
            LLVMValueRef t1 = llvm_base_offset(llvm, s, op->args[1], op->args[2], type);

            LLVMBuildStore(llvm->builder, val, t1);
            break;
        }
        case INDEX_op_brcond_i32:
        case INDEX_op_brcond_i64: {
            LLVMValueRef t0 = llvm_var_value(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var_value(llvm, s, op->args[1]);
            TCGCond cond = (TCGCond)op->args[2];
            TCGLabel *label = arg_label(op->args[3]);
            LLVMBasicBlockRef llvm_then_bb = llvm_bb_for_label(llvm, llvm_function, label);
            LLVMBasicBlockRef llvm_else_bb = LLVMAppendBasicBlock(llvm_function, llvm_unnamed);
            LLVMValueRef llvm_cond = LLVMBuildICmp(llvm->builder, llvm_int_predicate(cond), t0, t1, llvm_unnamed);

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
        case INDEX_op_qemu_ld_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            TCGMemOpIdx flags = (TCGMemOpIdx)op->args[2];
            TCGMemOp memop = get_memop(flags);
            LLVMTypeRef load_type = llvm_memop_type(memop);
            LLVMValueRef t1 = llvm_base_offset(llvm, s, op->args[1], 0, load_type);
            LLVMValueRef endian_val = LLVMBuildLoad(llvm->builder, t1, llvm_unnamed);
            LLVMValueRef signed_val = llvm_memop_bswap(llvm, endian_val, memop);
            LLVMValueRef val = (load_type == LLVMInt64Type()) ?
                        signed_val :
                        ((memop & MO_SIGN) ?
                             LLVMBuildSExt(llvm->builder, signed_val, LLVMInt64Type(), llvm_unnamed) :
                             LLVMBuildZExt(llvm->builder, signed_val, LLVMInt64Type(), llvm_unnamed));

            LLVMBuildStore(llvm->builder, val, t0);
            break;
        }
        case INDEX_op_qemu_st_i64: {
            LLVMValueRef t0 = llvm_var_value(llvm, s, op->args[0]);
            TCGMemOpIdx flags = (TCGMemOpIdx)op->args[2];
            TCGMemOp memop = get_memop(flags);
            LLVMTypeRef store_type = llvm_memop_type(memop);
            LLVMValueRef endian_val = (store_type == LLVMInt64Type()) ?
                        t0 :
                        LLVMBuildTrunc(llvm->builder, t0, store_type, llvm_unnamed);
            LLVMValueRef val = llvm_memop_bswap(llvm, endian_val, memop);
            LLVMValueRef t1 = llvm_base_offset(llvm, s, op->args[1], 0, store_type);

            LLVMBuildStore(llvm->builder, val, t1);
            break;
        }
        case INDEX_op_call: {
            int n_out = TCGOP_CALLO(op);
            int n_in = TCGOP_CALLI(op);
            void *func = (void *)op->args[n_out + n_in];
            TCGHelperInfo *helper = NULL;
            char name[32];
            LLVMValueRef llvm_helper;
            LLVMValueRef out;
            LLVMValueRef args[MAX_ARGS];
            int i;
            LLVMValueRef ret;

            if (helper_table)
                helper = g_hash_table_lookup(helper_table, (gpointer)func);
            if (!helper) {
                fprintf(stderr, "Unsupported func: %p\n", func);
                abort();
            }
            switch (n_out) {
            case 0:
                out = NULL;
                break;
            case 1:
                out = llvm_var(llvm, s, op->args[0]);
                break;
            default:
                fprintf(stderr, "Unsupported callo: %d\n", n_out);
                abort();
            }
            if (n_in > MAX_ARGS) {
                fprintf(stderr, "calli (%d) > MAX_ARGS (%d)\n", n_in, MAX_ARGS);
                abort();
            }
            for (i = 0; i < n_in; ++i)
                args[i] = llvm_var_value(llvm, s, op->args[n_out + i]);
            easy_snprintf(name, "helper_%s", helper->name);
            llvm_helper = LLVMGetNamedFunction(llvm->module, name);
            if (!llvm_helper) {
                LLVMTypeRef return_type = out ? LLVMGetElementType(LLVMTypeOf(out)) : LLVMVoidType();
                LLVMTypeRef arg_types[MAX_ARGS];

                for (i = 0; i < n_in; ++i)
                    arg_types[i] = LLVMTypeOf(args[i]);
                llvm_helper = LLVMAddFunction(llvm->module, name, LLVMFunctionType(return_type, arg_types, n_in, false));
            }
            ret = LLVMBuildCall(llvm->builder, llvm_helper, args, n_in, llvm_unnamed);
            if (out)
                LLVMBuildStore(llvm->builder, ret, out);

            break;
        }
        case INDEX_op_exit_tb: {
            int64_t ret = (int64_t)op->args[0];

            // cpu-exec.c:177
            if ((ret & TB_EXIT_MASK) > TB_EXIT_IDX1) {
                // Interrupts, instruction counting and block linking
                // must not happen in stand-alone mode.
                LLVMBuildTrap(llvm->builder);
                LLVMBuildUnreachable(llvm->builder);
            } else
                LLVMBuildRetVoid(llvm->builder);
            llvm_bb = NULL;
            break;
        }
        case INDEX_op_add_i32:
        case INDEX_op_add_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildAdd, op->args[2]);
            break;
        case INDEX_op_sub_i32:
        case INDEX_op_sub_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildSub, op->args[2]);
            break;
        case INDEX_op_neg_i32:
        case INDEX_op_neg_i64:
            llvm_un_op(llvm, s, op->args[0], &LLVMBuildNeg, op->args[1]);
            break;
        case INDEX_op_mul_i32:
        case INDEX_op_mul_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildMul, op->args[2]);
            break;
        case INDEX_op_shl_i32:
        case INDEX_op_shl_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildShl, op->args[2]);
            break;
        case INDEX_op_shr_i32:
        case INDEX_op_shr_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildLShr, op->args[2]);
            break;
        case INDEX_op_sar_i32:
        case INDEX_op_sar_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildAShr, op->args[2]);
            break;
        case INDEX_op_and_i32:
        case INDEX_op_and_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildAnd, op->args[2]);
            break;
        case INDEX_op_or_i32:
        case INDEX_op_or_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildOr, op->args[2]);
            break;
        case INDEX_op_xor_i32:
        case INDEX_op_xor_i64:
            llvm_bin_op(llvm, s, op->args[0], op->args[1], &LLVMBuildXor, op->args[2]);
            break;
        case INDEX_op_not_i32:
        case INDEX_op_not_i64:
            llvm_un_op(llvm, s, op->args[0], &LLVMBuildNot, op->args[1]);
            break;
        case INDEX_op_mov_i32:
        case INDEX_op_mov_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var_value(llvm, s, op->args[1]);

            // Doc says t0 and t1 must have the same size, but despite that
            // tcg_gen_extrl_i64_i32() puts 64-bit t1 into 32-bit t0.
            if (LLVMTypeOf(t0) == LLVMPointerType(LLVMInt32Type(), 0) &&
                    LLVMTypeOf(t1) == LLVMInt64Type())
                t1 = LLVMBuildTrunc(llvm->builder, t1, LLVMInt32Type(), llvm_unnamed);

            LLVMBuildStore(llvm->builder, t1, t0);
            break;
        }
        case INDEX_op_ext8s_i32:
        case INDEX_op_ext8s_i64:
        case INDEX_op_ext16s_i32:
        case INDEX_op_ext16s_i64:
        case INDEX_op_ext32s_i64:
            llvm_cast_op(llvm, s, op->args[0], &LLVMBuildSExt, op->args[1]);
            break;
        case INDEX_op_ext8u_i32:
        case INDEX_op_ext8u_i64:
        case INDEX_op_ext16u_i32:
        case INDEX_op_ext16u_i64:
        case INDEX_op_ext32u_i64:
            llvm_cast_op(llvm, s, op->args[0], &LLVMBuildZExt, op->args[1]);
            break;
        case INDEX_op_goto_tb:
            // Simply execute the next instructions, since in stand-alone mode
            // translation block linking should not happen.
            break;
        case INDEX_op_deposit_i32:
        case INDEX_op_deposit_i64: {
            LLVMValueRef dest = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var_value(llvm, s, op->args[1]);
            LLVMValueRef t2 = llvm_var_value(llvm, s, op->args[2]);
            uint16_t pos = (int32_t)op->args[3];
            LLVMTypeRef type = (op->opc == INDEX_op_deposit_i32) ? LLVMInt32Type() : LLVMInt64Type();
            LLVMValueRef llvm_pos = LLVMConstInt(type, pos, false);
            uint8_t len = (int32_t)op->args[4];
            uint64_t mask = ((1ULL << len) - 1) << pos;
            LLVMValueRef t1_mask = LLVMConstInt(type, ~mask, false);
            LLVMValueRef t1_masked = LLVMBuildAnd(llvm->builder, t1, t1_mask, llvm_unnamed);
            LLVMValueRef t2_shifted = LLVMBuildShl(llvm->builder, t2, llvm_pos, llvm_unnamed);
            LLVMValueRef t2_mask = LLVMConstInt(type, mask, false);
            LLVMValueRef t2_masked = LLVMBuildAnd(llvm->builder, t2_shifted, t2_mask, llvm_unnamed);
            LLVMValueRef val = LLVMBuildOr(llvm->builder, t1_masked, t2_masked, llvm_unnamed);

            LLVMBuildStore(llvm->builder, val, dest);
            break;
        }
        case INDEX_op_setcond_i32:
        case INDEX_op_setcond_i64: {
            LLVMValueRef dest = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var_value(llvm, s, op->args[1]);
            LLVMValueRef t2 = llvm_var_value(llvm, s, op->args[2]);
            TCGCond cond = (TCGCond)op->args[3];
            LLVMValueRef llvm_cond = LLVMBuildICmp(llvm->builder, llvm_int_predicate(cond), t1, t2, llvm_unnamed);
            LLVMValueRef val = LLVMBuildZExt(llvm->builder, llvm_cond, LLVMGetElementType(LLVMTypeOf(dest)), llvm_unnamed);

            LLVMBuildStore(llvm->builder, val, dest);
            break;
        }
        case INDEX_op_ext_i32_i64:
            llvm_cast_op(llvm, s, op->args[0], &LLVMBuildSExt, op->args[1]);
            break;
        case INDEX_op_extu_i32_i64:
            llvm_cast_op(llvm, s, op->args[0], &LLVMBuildZExt, op->args[1]);
            break;
        case INDEX_op_discard:
            // Ignore, this is an analysis hint.
            break;
        case INDEX_op_bswap16_i32:
        case INDEX_op_bswap16_i64:
        case INDEX_op_bswap32_i32:
        case INDEX_op_bswap32_i64: {
            LLVMValueRef t0 = llvm_var(llvm, s, op->args[0]);
            LLVMValueRef t1 = llvm_var_value(llvm, s, op->args[1]);
            LLVMTypeRef type = (op->opc == INDEX_op_bswap16_i32 || op->opc == INDEX_op_bswap16_i64) ? LLVMInt16Type() : LLVMInt32Type();
            LLVMValueRef truncated = LLVMBuildTrunc(llvm->builder, t1, type, llvm_unnamed);
            LLVMValueRef swapped = LLVMBuildBSwap(llvm->builder, truncated, llvm_unnamed);
            LLVMValueRef extended = LLVMBuildZExt(llvm->builder, swapped, LLVMTypeOf(t1), llvm_unnamed);

            LLVMBuildStore(llvm->builder, extended, t0);
            break;
        }
        case INDEX_op_bswap64_i64:
            llvm_un_op(llvm, s, op->args[0], &LLVMBuildBSwap, op->args[1]);
            break;
        default:
            fprintf(stderr, "Unsupported op: %s\n", def->name);
            return NULL;
        }
    }
    llvm_register_pc(llvm, pc, llvm_function, llvm_disasm);
    return llvm_function;
}
