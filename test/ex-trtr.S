#include "test.h"
.Lzero:
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Ltrtr:
    trtr .Lop1+3(1),.Lop2
.Lop1:
    .byte 0,1,2,3
.Lop2:
    .byte 0
    .byte 0xbb
    .fill 0xfe,1,0
.Lx:
    lghi 2,3
    ex 2,.Ltrtr
    jge .Lbadcc
    clfi 1,.Lop1+1-.Lzero
    jne .Lbadr1
    clfi 2,0xbb
    jne .Lbadr2
    xgr 2,2
    j .Lhalt
.Lbadcc:
    lgfi 2,0xbadcc
    j .Lhalt
.Lbadr1:
    lgfi 2,0xbad01
    j .Lhalt
.Lbadr2:
    lgfi 2,0xbad02
    j .Lhalt
TEST_END
