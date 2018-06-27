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

LLVMValueRef LLVMBuildBSwap(LLVMBuilderRef B, LLVMValueRef V,
                            const char *name)
{
    BasicBlock *BB = unwrap(B)->GetInsertBlock();
    Module *M = BB->getParent()->getParent();
    Type *Ty = unwrap(V)->getType();
    Type *Tys[] = { Ty };
    Value *TheFn = Intrinsic::getDeclaration(M, Intrinsic::bswap, Tys);
    Value *Args[] = { unwrap(V) };

    return wrap(unwrap(B)->CreateCall(TheFn, Args, name));
}

LLVMValueRef LLVMBuildTrap(LLVMBuilderRef B)
{
    BasicBlock *BB = unwrap(B)->GetInsertBlock();
    Module *M = BB->getParent()->getParent();
    Value *TheFn = Intrinsic::getDeclaration(M, Intrinsic::trap, {});

    return wrap(unwrap(B)->CreateCall(TheFn, {}));
}

LLVMTypeRef LLVMHugeArrayType(LLVMTypeRef ElementType, uint64_t ElementCount) {
  return wrap(ArrayType::get(unwrap(ElementType), ElementCount));
}
