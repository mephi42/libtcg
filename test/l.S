#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lmagic:
    .long 0x12345678
.Lx:
    l 2,.Lmagic
    afi 2,-0x12345678
    j .Lhalt
TEST_END
