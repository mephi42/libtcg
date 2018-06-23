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
    struct S390CPU *s390_cpu;
    struct llvm llvm;
    int pc_begin, pc_end;
    int pc;
    int debug_pc = -1;
    bool verbose = debug_pc >= 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s in-file out-file\n", argv[0]);
        return EXIT_FAILURE;
    }

    cpu = libtcg_init(argv[1]);
    if (!cpu) {
        fprintf(stderr, "libtcg_init() failed\n");
        return EXIT_FAILURE;
    }
    s390_cpu = S390_CPU(cpu);

    llvm_init(&llvm, cpu, argv[1]);

    if (debug_pc >= 0) {
        pc_begin = debug_pc;
        pc_end = debug_pc + 2;
    } else {
        pc_begin = 0;
        pc_end = llvm.image_size;
    }
    for (pc = pc_begin; pc < pc_end; pc += 2) {
        struct TCGContext *s;
        LLVMValueRef llvm_function;

        fprintf(stderr, "pc = 0x%x\n", pc);
        s390_cpu->env.psw.addr = pc;
        s = libtcg_gen(cpu);
        if (!s) {
            fprintf(stderr, "libtcg_gen() failed\n");
            abort();
        }
        if (verbose)
            tcg_dump_ops(s);
        llvm_function = llvm_convert_tb(&llvm, s, pc);
        if (!llvm_function)
            abort();
        if (verbose)
            LLVMDumpValue(llvm_function);
    }

    if (verbose)
        LLVMDumpValue(llvm.dispatch);
    if (LLVMVerifyModule(llvm.module, LLVMPrintMessageAction, NULL))
        abort();
    if (debug_pc < 0)
        LLVMWriteBitcodeToFile(llvm.module, argv[2]);
}
