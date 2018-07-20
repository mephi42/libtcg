#include "test.h"
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Lop1:
    .quad 0x0123456789abcdef
.Lop2:
    .quad 0
.Lparmlist:
.Lreplacement:
    .quad 0xfedcba9876543210
.Lstore:
    .quad 0x7777777777777777
.Lx:
    lghi 0,0x0301
    la 1,.Lparmlist
    lg 3,.Lop1
    .insn ssf,0xc80200000000,.Lop1,.Lop2,3
    jne .Lbadcc
    clc .Lop1(8),.Lreplacement
    jne .Lbadop1
    j .Lhalt
    clc .Lop2(8),.Lstore
    jne .Lbadop2
.Lhalt:
    lpswe .Lhalt_psw
.Lbadcc:
    llilf 2,0xbadcc
    j .Lhalt
.Lbadop1:
    llilf 2,0xbad01
    j .Lhalt
.Lbadop2:
    llilf 2,0xbad02
    j .Lhalt
    .org LINUX_HEAD_END
