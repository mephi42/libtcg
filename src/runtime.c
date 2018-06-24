#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qemu/osdep.h"

#include "qemu/log.h"
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

static void configure_logging(const char *log_mask)
{
    // vl.c:4035
    if (log_mask) {
        int mask = qemu_str_to_log_mask(log_mask);

        if (!mask) {
            qemu_print_log_usage(stderr);
            exit(EXIT_FAILURE);
        }
        qemu_set_log(mask);
    } else {
        qemu_set_log(0);
    }
}

int main(int argc, char **argv)
{
    int i;
    int j;
    int flags = 0;
    const char *log_mask = NULL;

    for (i = 1, j = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr, "%s: -d: requires an argument\n", argv[0]);
                return EXIT_FAILURE;
            }
            log_mask = argv[i];
        } else {
            argv[j] = argv[i];
            j++;
        }
    }
    argc = j;
    argv[argc] = NULL;

    configure_logging(log_mask);
    if (qemu_loglevel & CPU_LOG_TB_CPU)
        flags |= FLAG_CPU_DUMP_STATE;

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
