#include <stdio.h>
#include <stdlib.h>

#include "qemu/osdep.h"

#include "libtcg.h"
#include "llvm-c/Analysis.h"
#include "llvm-c/BitWriter.h"
#include "llvm-c/Core.h"
#include "tcg/tcg.h"
#include "tcg2llvm.h"

int main(int argc, char **argv)
{
    struct CPUState *cpu;
    struct llvm llvm;
    int pc;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s in-file out-file\n", argv[0]);
        return EXIT_FAILURE;
    }

    cpu = libtcg_init(argv[1]);
    if (!cpu) {
        fprintf(stderr, "libtcg_init() failed\n");
        return EXIT_FAILURE;
    }

    llvm_init(&llvm, argv[1]);

    for (pc = 0; pc < llvm.image_size; pc += 2) {
        struct S390CPU *s390_cpu;
        struct TCGContext *s;
        LLVMValueRef llvm_function;

        s390_cpu = S390_CPU(cpu);
        s390_cpu->env.psw.addr = pc;
        s = libtcg_gen(cpu);
        if (!s) {
            fprintf(stderr, "libtcg_gen() failed\n");
            abort();
        }
        tcg_dump_ops(s);
        llvm_function = llvm_convert_tb(&llvm, s, pc);
        if (!llvm_function)
            abort();
        LLVMDumpValue(llvm_function);
    }

    llvm_add_data(&llvm, cpu);
    if (LLVMVerifyModule(llvm.module, LLVMPrintMessageAction, NULL))
        abort();
    LLVMWriteBitcodeToFile(llvm.module, argv[2]);
}
