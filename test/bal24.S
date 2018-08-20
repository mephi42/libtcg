#include "test.h"
.Lzero:
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad (0xa << PSW_SHIFT_MASK_PM),.Lx
    .org LOWCORE_END
.Linitial:
    .quad 0xffffffffffffffff
.Lexpected:
    .quad 0xffffffff00000000 | (0x2 << 30) | (0x1 << 28) | (0xa << 24) | (.Ly - .Lzero)
.Lx:
    lg 2,.Linitial
    cfi 2,2
    bal 2,.Ly
.Ly:
    xg 2,.Lexpected
    j .Lhalt
TEST_END
