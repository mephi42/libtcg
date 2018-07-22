#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Ldata:
    .quad 0x000000000025594c
.Lx:
    xgr 2,2
    cvb 2,.Ldata
    xilf 2,0x000063fa
    j .Lhalt
TEST_END
