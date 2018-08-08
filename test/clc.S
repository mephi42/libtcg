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
    lghi 2,-1
    clc .Lmagic1(4),.Lmagic2
    ipm 2
    xihf 2,0xffffffff
    xilf 2,0x20ffffff
    j .Lhalt
TEST_END
