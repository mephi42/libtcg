#include "libtcg.h"

#include "qemu/osdep.h"
#include "tcg/tcg.h"
#include "exec/exec-all.h"

int main(int argc, char ** argv)
{
    struct S390CPU *cpu;
    struct TranslationBlock *tb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    cpu = S390_CPU(libtcg_init(argv[1]));
    if (!cpu) {
        fprintf(stderr, "%s: libtcg_init() failed\n", argv[0]);
        return EXIT_FAILURE;
    }

    // translate-all.c:1250
    tb = tcg_tb_alloc(tcg_ctx);
    if (!tb) {
        fprintf(stderr, "%s: tcg_tb_alloc() failed\n", argv[0]);
        return EXIT_FAILURE;
    }
    // translate-all.c:1272
    cpu_get_tb_cpu_state(&cpu->env, &tb->pc, &tb->cs_base, &tb->flags);
    // translate-all.c:1285
    tcg_func_start(tcg_ctx);
    // translate-all.c:1288
    gen_intermediate_code(&cpu->parent_obj, tb);

    // print results
    tcg_dump_ops(tcg_ctx);
}
