#include "llvm-core-extras.h"

#include "llvm/IR/IRBuilder.h"

using namespace llvm;

LLVMValueRef LLVMBuildMemCpy(LLVMBuilderRef B, LLVMValueRef Dst,
                             unsigned DstAlign, LLVMValueRef Src,
                             unsigned SrcAlign, uint64_t Size)
{
    return wrap(unwrap(B)->CreateMemCpy(unwrap(Dst), DstAlign, unwrap(Src),
                                        SrcAlign, Size));
}
