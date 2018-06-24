#include "test.h"
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lx:
    la 2,.Ly
    balr 14,2
.Ly:
    sr 2,14
    xilf 2,0x80000000
    lpswe .Lhalt_psw
