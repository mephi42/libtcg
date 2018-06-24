#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qemu/osdep.h"

#include "qom/cpu.h"
#include "target/s390x/cpu.h"
#include "target/s390x/internal.h"

extern struct CPUState cpu;
extern char memory[];
extern const char memory_init[];
extern const int32_t memory_init_size;
extern void dispatch();

#define FLAG_CPU_DUMP_STATE 0x1

__attribute__((always_inline))
static inline void dispatch_loop(int flags)
{
    sigsetjmp(cpu.jmp_env, 0);
    while (true) {
        if (flags & FLAG_CPU_DUMP_STATE)
            s390_cpu_dump_state(&cpu, stderr, fprintf, 0);
        if (cpu.exception_index < 0)
            dispatch();
        else
            s390_cpu_do_interrupt(&cpu);
    }
}

int main(int argc, char **argv)
{
    int i;
    int j;
    int flags = 0;

    for (i = 1, j = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr, "%s: -d: requires an argument\n", argv[0]);
                return EXIT_FAILURE;
            }
            if (strcmp(argv[i], "cpu") == 0)
                flags |= FLAG_CPU_DUMP_STATE;
            else {
                fprintf(stderr, "%s: -d: unsupported value: %s\n", argv[0], argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            argv[j] = argv[i];
            j++;
        }
    }
    argc = j;
    argv[argc] = NULL;

    memcpy(memory, memory_init, memory_init_size);

    switch (flags) {
#define DISPATCH_LOOP_CASE(n) \
    case n: \
        dispatch_loop(n); \
        break
    DISPATCH_LOOP_CASE(0);
    DISPATCH_LOOP_CASE(1);
#undef DISPATCH_LOOP_CASE
    default:
        abort();
    }
}
