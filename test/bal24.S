#include "test.h"
.zero:
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad (0xa << PSW_SHIFT_MASK_PM),.Lx
    .org LOWCORE_END
.Lx:
    lghi 2,1
    cfi 2,2
    bal 2,.Ly
.Ly:
    xilf 2,((0x2 << 30) | (0x1 << 28) | (0xa << 24) | (.Ly - .zero))
    j .Lhalt
TEST_END
