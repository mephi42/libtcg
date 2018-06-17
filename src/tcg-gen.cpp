#include <iostream>

#include "libtcg.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

int main(int argc, char ** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file" << std::endl;
        return EXIT_FAILURE;
    }

    auto cpu = libtcg_init(argv[1]);
    if (!cpu) {
        std::cerr << argv[0] << ": libtcg_init() failed" << std::endl;
        return EXIT_FAILURE;
    }

    auto s = libtcg_gen(cpu);
    if (!s) {
        std::cerr << argv[0] << ": libtcg_gen() failed" << std::endl;
        return EXIT_FAILURE;
    }

    libtcg_dump_ops(s);

    llvm::LLVMContext llvm_context;
    auto bb = llvm::BasicBlock::Create(llvm_context);
    llvm::IRBuilder<> builder(bb);
}
