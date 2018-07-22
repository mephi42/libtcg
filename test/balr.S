#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lx:
    la 2,.Ly
    balr 14,2
.Ly:
    sr 2,14
    xilf 2,0x80000000
    j .Lhalt
TEST_END
