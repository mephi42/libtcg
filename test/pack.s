#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Ldata:
    .byte 0xf1,0xf2,0xf3,0xc4
.Lx:
    xgr 2,2
    pack .Ldata(4),.Ldata(4)
    l 2,.Ldata
    xilf 2,0x0001234c
    lpswe .Lhalt_psw
