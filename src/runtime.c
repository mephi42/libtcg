#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
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

int main()
{
    memcpy(memory, memory_init, memory_init_size);
    sigsetjmp(cpu.jmp_env, 0);
    while (true) {
        if (cpu.exception_index < 0)
            dispatch();
        else
            s390_cpu_do_interrupt(&cpu);
    }
}
