#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Larray:
    .byte 1,2,3,4,5,6,7,8,9,10
    .org 0x280
.Lx:
    la 1,.Larray
    xc 0(9,1),0(1)
    lg 2,0(1)
    ag 2,2(1)
    aghi 2,-10
    lpswe .Lhalt_psw
