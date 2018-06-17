#include <iostream>

#include "libtcg.h"

int main(int argc, char ** argv)
{
    struct CPUState *cpu;
    struct TCGContext *s;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file" << std::endl;
        return EXIT_FAILURE;
    }

    cpu = libtcg_init(argv[1]);
    if (!cpu) {
        std::cerr << argv[0] << ": libtcg_init() failed" << std::endl;
        return EXIT_FAILURE;
    }

    s = libtcg_gen(cpu);
    if (!s) {
        std::cerr << argv[0] << ": libtcg_gen() failed" << std::endl;
        return EXIT_FAILURE;
    }

    // print results
    libtcg_dump_ops(s);
}
