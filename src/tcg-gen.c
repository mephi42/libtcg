#include "libtcg.h"

#include "qemu/osdep.h"
#include "tcg/tcg.h"
#include "exec/exec-all.h"

int main(int argc, char ** argv)
{
    struct CPUState *cpu;
    struct TCGContext *ctx;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    cpu = libtcg_init(argv[1]);
    if (!cpu) {
        fprintf(stderr, "%s: libtcg_init() failed\n", argv[0]);
        return EXIT_FAILURE;
    }

    ctx = libtcg_gen(cpu);
    if (!ctx) {
        fprintf(stderr, "%s: libtcg_gen() failed\n", argv[0]);
        return EXIT_FAILURE;
    }

    // print results
    tcg_dump_ops(ctx);
}
