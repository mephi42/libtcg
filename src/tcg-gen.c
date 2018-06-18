#include <stdio.h>
#include <stdlib.h>

#include "qemu/osdep.h"

#include "libtcg.h"
#include "llvm-c/Core.h"
#include "tcg/tcg.h"
#include "tcg2llvm.h"

int main(int argc, char ** argv)
{
    struct CPUState *cpu;
    struct TCGContext *s;
    struct S390CPU *s390_cpu;
    struct llvm llvm;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    cpu = libtcg_init(argv[1]);
    if (!cpu) {
        fprintf(stderr, "libtcg_init() failed\n");
        return EXIT_FAILURE;
    }

    s = libtcg_gen(cpu);
    if (!s) {
        fprintf(stderr, "libtcg_gen() failed\n");
        return EXIT_FAILURE;
    }

    tcg_dump_ops(s);

    s390_cpu = S390_CPU(cpu);
    llvm_init(&llvm, argv[1]);
    llvm_convert_tb(&llvm, s, s390_cpu->env.psw.addr);
    LLVMDumpModule(llvm.module);
}
