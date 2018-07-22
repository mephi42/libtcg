#ifndef TEST_H
#define TEST_H

/* target/s390x/cpu.h */
#define PSW_MASK_WAIT           0x0002000000000000ULL
#define PSW_MASK_CC             0x0000300000000000ULL
#define PSW_SHIFT_MASK_CC       44
#define PSW_MASK_PM             0x00000F0000000000ULL
#define PSW_SHIFT_MASK_PM       40
#define PSW_MASK_32             0x0000000080000000ULL

/* hw/s390x/ipl.c */
#define LOWCORE_IPL_PSW         0x0

/* target/s390x/internal.h */
#define LOWCORE_PROGRAM_OLD_PSW 0x150
#define LOWCORE_RESTART_NEW_PSW 0x1a0
#define LOWCORE_PROGRAM_NEW_PSW 0x1d0
#define LOWCORE_END             0x200

/* arch/s390/kernel/head.S */
#define LINUX_STARTUP           0x10000
#define LINUX_HEAD_END          0x11000

.macro TEST_END
.Lhalt:
    lpswe .Lhalt_psw
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
    .org LINUX_STARTUP
    j .Lx
    .org LINUX_HEAD_END
.endm

#endif
