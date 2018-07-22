#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Ldata:
    .byte 0xaa,0xaa,0xf1,0xf2,0xf3,0xc4,0xaa,0xaa
.Lx:
    pack .Ldata+2(4),.Ldata+2(4)
    lg 2,.Ldata
    xihf 2,0xaaaa0001
    xilf 2,0x234caaaa
    j .Lhalt
TEST_END
