#include "test.h"
.zero:
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad (0x5 << PSW_SHIFT_MASK_PM),.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lbalr:
    balr 0,2
.Lx:
    lghi 1,0x20
    la 2,.Ly
    cfi 1,1
    exrl 1,.Lbalr
.Ly:
    xilf 2,((0x3 << 30) | (0x2 << 28) | (0x5 << 24) | (.Ly - .zero))
    lpswe .Lhalt_psw
    .org LINUX_HEAD_END
