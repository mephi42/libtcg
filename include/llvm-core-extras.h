#ifndef LLVM_CORE_EXTRAS_H
#define LLVM_CORE_EXTRAS_H

#include <stdint.h>

#include "llvm-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

LLVMValueRef LLVMBuildMemCpy(LLVMBuilderRef B, LLVMValueRef Dst,
                             unsigned DstAlign, LLVMValueRef Src,
                             unsigned SrcAlign, uint64_t Size);

LLVMValueRef LLVMBuildBSwap(LLVMBuilderRef B, LLVMValueRef V,
                            const char *name);

LLVMValueRef LLVMBuildTrap(LLVMBuilderRef B);

LLVMTypeRef LLVMHugeArrayType(LLVMTypeRef ElementType, uint64_t ElementCount);

#ifdef __cplusplus
}
#endif

#endif
