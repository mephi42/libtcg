#include "test.h"
.Lzero:
    .org LOWCORE_IPL_PSW
    .long 0,.Lx
    .org LOWCORE_RESTART_NEW_PSW
    .quad PSW_MASK_32,.Lx
    .org LOWCORE_END
.Lhalt_psw:
    .quad PSW_MASK_WAIT,0
.Ltrt:
    trt .Lop1(1),.Lop2
.Lop1:
    .asciz "hello"
.Lop2:
    .byte 0xaa
    .fill 0xff,1,0
.Lx:
    lghi 2,5
    ex 2,.Ltrt
    jle .Lbadcc
    clfi 1,.Lop1+5-.Lzero
    jne .Lbadr1
    clfi 2,0xaa
    jne .Lbadr2
    xgr 2,2
.Lhalt:
    lpswe .Lhalt_psw
.Lbadcc:
    lgfi 2,0xbadcc
    j .Lhalt
.Lbadr1:
    lgfi 2,0xbad01
    j .Lhalt
.Lbadr2:
    lgfi 2,0xbad02
    j .Lhalt
    .org LINUX_HEAD_END
