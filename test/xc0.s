    .org 0x1a0
    .quad 0,.Lx
    .org 0x200
.Lhalt_psw:
    .quad 0x0002000000000000,0
.Larray:
    .byte 1,2,3,4,5,6,7,8,9,10
    .org 0x280
.Lx:
    la 1,.Larray
    xc 0(9,1),0(1)
    lg 2,0(1)
    ag 2,2(1)
    aghi 2,-10
    lpswe .Lhalt_psw
