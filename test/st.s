    .org 0x1a0
    .quad 0x0,.Lx
    .org 0x200
.Lmagic:
    .long 0x12345678
.Lhalt_psw:
    .quad 0x0002000000000000,0
.Lx:
    lgfi 2,0x12345678
    st 2,.Lmagic
    s 2,.Lmagic
    lpswe .Lhalt_psw
