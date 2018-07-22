#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lmagic1:
    .long 0x55555555
.Lmagic2:
    .long 0x44444444
.Lx:
    xgr 2,2
    clc .Lmagic1(4),.Lmagic2
    ipm 2
    xilf 2,0x20000000
    j .Lhalt
TEST_END
