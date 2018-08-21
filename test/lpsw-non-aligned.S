#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_PROGRAM_NEW_PSW
    .quad PSW_MASK_32,.Lpgm
    .org LOWCORE_END
    .align 8
.Lpgm:
    lpswe LOWCORE_PROGRAM_OLD_PSW
.Lx:
    lpsw 4
    lpswe 4
    xr 2,2
    j .Lhalt
TEST_END
