#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_PROGRAM_NEW_PSW
    .quad PSW_MASK_32,.Lpgm
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lpgm:
    lpswe LOWCORE_PROGRAM_OLD_PSW
.Lx:
    xgr 1,1
    lgfi 2,0x7fffffff
    mvc 0(1,1),0(2)
    xr 2,2
    lpswe .Lhalt_psw
    .org LINUX_HEAD_END
