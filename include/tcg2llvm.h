#ifndef TCG2LLVM_H
#define TCG2LLVM_H

#include <stdint.h>

#include "llvm-c/Core.h"

struct TCGContext;

#define MAX_LABELS 16
#define MAX_LOCALS 16

struct llvm {
    LLVMModuleRef module;
    LLVMValueRef memory;
    LLVMValueRef cpu;
    LLVMValueRef cpu_env;
    LLVMBuilderRef builder;
    LLVMBasicBlockRef labels[MAX_LABELS];
    LLVMValueRef locals[MAX_LOCALS];
};

void llvm_init(struct llvm *llvm, const char *module_id);
void llvm_convert_tb(struct llvm *llvm, struct TCGContext *s, uint64_t pc);

#endif
