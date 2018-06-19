#ifndef TCG2LLVM_H
#define TCG2LLVM_H

#include <stdint.h>

#include "llvm-c/Core.h"

struct CPUState;
struct TCGContext;

#define MAX_LABELS 16
#define MAX_LOCALS 16
#define MAX_ARGS 16

struct llvm {
    LLVMModuleRef module;
    LLVMValueRef memory;
    LLVMValueRef cpu;
    LLVMValueRef cpu_env;
    LLVMBuilderRef builder;
    LLVMBasicBlockRef labels[MAX_LABELS];
    LLVMValueRef locals[MAX_LOCALS];
    int image_size;
    LLVMValueRef memory_init;
};

void llvm_init(struct llvm *llvm, const char *path);
LLVMValueRef llvm_convert_tb(struct llvm *llvm, struct TCGContext *s, uint64_t pc);
void llvm_add_data(struct llvm *llvm, struct CPUState *cpu);

#endif
