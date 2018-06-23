    .org 0x1a0
    .quad 0x0000000080000000,.Lx
    .org 0x200
.Lhalt_psw:
    .quad 0x0002000000000000,0
.Lx:
    la 2,.Ly
    balr 14,2
.Ly:
    sr 2,14
    xilf 2,0x80000000
    lpswe .Lhalt_psw
