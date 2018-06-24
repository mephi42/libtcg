#include "test.h"
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lentry
    .org LOWCORE_PROGRAM_NEW_PSW
    .quad PSW_MASK_32,.Lpgm
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lpgm:
    lpswe 0x150
.Lentry:
    xgr 1,1
    llilf 2,0x7fffffff
    mvc 0(1,1),0(2)
    xr 2,2
    lpswe .Lhalt_psw
