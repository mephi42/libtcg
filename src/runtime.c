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
extern struct S390CPUDef cpu_def;
char *memory;
extern const char memory_init[];
extern const int32_t memory_init_size;
extern void dispatch();
extern const char *disasm();

#define FLAG_CPU_DUMP_STATE 0x1
#define FLAG_CPU_DUMP_DISASM 0x2

__attribute__((always_inline))
static inline void dispatch_loop(int flags)
{
    sigsetjmp(cpu.jmp_env, 0);
    while (true) {
        if (cpu.exception_index < 0) {
            if (flags & FLAG_CPU_DUMP_DISASM)
                qemu_log("%s", disasm());
            if (flags & FLAG_CPU_DUMP_STATE)
                s390_cpu_dump_state(&cpu, qemu_logfile, fprintf, 0);
            dispatch();
        } else
            s390_cpu_do_interrupt(&cpu);
    }
}

static void configure_logging(const char *log_file, const char *log_mask)
{
    // vl.c:4031
    if (log_file)
        qemu_set_log_filename(log_file, NULL);

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
    const char *log_file = NULL;
    const char *log_mask = NULL;

    for (i = 1, j = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr, "%s: -d: requires an argument\n", argv[0]);
                return EXIT_FAILURE;
            }
            log_mask = argv[i];
        } else if (strcmp(argv[i], "-D") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr, "%s: -D: requires an argument\n", argv[0]);
                return EXIT_FAILURE;
            }
            log_file = argv[i];
        } else if (strcmp(argv[i], "-cpu") == 0) {
            i++;
            if (i >= argc) {
                fprintf(stderr, "%s: -cpu: requires an argument\n", argv[0]);
                return EXIT_FAILURE;
            }
            cpu_def.type = strtol(argv[i], NULL, 16);
            S390_CPU(&cpu)->env.cpuid = s390_cpuid_from_cpu_model(S390_CPU(&cpu)->model);
        } else if (strcmp(argv[i], "-disasm") == 0) {
            flags |= FLAG_CPU_DUMP_DISASM;
        } else {
            argv[j] = argv[i];
            j++;
        }
    }
    argc = j;
    argv[argc] = NULL;

    configure_logging(log_file, log_mask);
    if (qemu_loglevel_mask(CPU_LOG_TB_CPU))
        flags |= FLAG_CPU_DUMP_STATE;

    memory = malloc(ram_size);
    if (!memory)
        abort();
    memcpy(memory, memory_init, memory_init_size);

    switch (flags) {
#define DISPATCH_LOOP_CASE(n) \
    case n: \
        dispatch_loop(n); \
        break
    DISPATCH_LOOP_CASE(0);
    DISPATCH_LOOP_CASE(1);
    DISPATCH_LOOP_CASE(2);
    DISPATCH_LOOP_CASE(3);
#undef DISPATCH_LOOP_CASE
    default:
        abort();
    }
}
