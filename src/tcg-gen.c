#include <stdio.h>
#include <stdlib.h>

#include "libtcg.h"
#include "llvm-c/Core.h"

int main(int argc, char ** argv)
{
    struct CPUState *cpu;
    struct TCGContext *s;
    LLVMBuilderRef llvm_builder;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    cpu = libtcg_init(argv[1]);
    if (!cpu) {
        fprintf(stderr, "%s: libtcg_init() failed\n", argv[0]);
        return EXIT_FAILURE;
    }

    s = libtcg_gen(cpu);
    if (!s) {
        fprintf(stderr, "%s: libtcg_gen() failed\n", argv[0]);
        return EXIT_FAILURE;
    }

    libtcg_dump_ops(s);

    llvm_builder = LLVMCreateBuilder();
}
