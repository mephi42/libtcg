#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Ldata:
    .quad 0x000000000025594c
.Lx:
    xgr 2,2
    cvb 2,.Ldata
    xilf 2,0x000063fa
    lpswe .Lhalt_psw
    .org LINUX_HEAD_END
