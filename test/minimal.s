    .org 0x1a0
    .quad 0,.Lx
    .org 0x200
.Lhalt_psw:
    .quad 0x0002000000000000,0
.Lx:
    xr 2,2
    lpswe .Lhalt_psw
