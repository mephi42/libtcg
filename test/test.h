#ifndef TEST_H
#define TEST_H

/* target/s390x/cpu.h */
#define PSW_MASK_32             0x0000000080000000ULL
#define PSW_MASK_WAIT           0x0002000000000000ULL

/* target/s390x/internal.h */
#define LOWCORE_RESTART_NEW_PSW 0x1a0
#define LOWCORE_PROGRAM_NEW_PSW 0x1d0
#define LOWCORE_END             0x200

#endif