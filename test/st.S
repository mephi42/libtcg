#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lmagic:
    .long 0x12345678
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lx:
    lgfi 2,0x12345678
    st 2,.Lmagic
    s 2,.Lmagic
    lpswe .Lhalt_psw
    .org LINUX_HEAD_END
