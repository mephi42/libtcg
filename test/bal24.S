#include "test.h"
.zero:
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad (0x1 << PSW_SHIFT_MASK_CC) | (0xa << PSW_SHIFT_MASK_PM),.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lx:
    bal 2,.Ly
.Ly:
    xilf 2,((0x2 << 30) | (0x1 << 28) | (0xa << 24) | (.Ly - .zero))
    lpswe .Lhalt_psw
