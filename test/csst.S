#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lop1:
    .quad 0x0123456789abcdef
.Lop2:
    .quad 0
.Lparmlist:
.Lreplacement:
    .quad 0xfedcba9876543210
    .quad 0
.Lstore:
    .quad 0x7777777777777777
    .quad 0
.Lx:
    lghi 0,0x0301
    la 1,.Lparmlist
    lg 3,.Lop1
    csst .Lop1,.Lop2,3
    jne .Lbadcc
    clc .Lop1(8),.Lreplacement
    jne .Lbadop1
    clc .Lop2(8),.Lstore
    jne .Lbadop2
    j .Lhalt
.Lbadcc:
    lgfi 2,0xbadcc
    j .Lhalt
.Lbadop1:
    lgfi 2,0xbad01
    j .Lhalt
.Lbadop2:
    lgfi 2,0xbad02
    j .Lhalt
TEST_END
