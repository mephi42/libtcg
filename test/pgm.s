    .org 0x1a0
    .quad 0,.Lentry
    .org 0x1d0
    .quad 0,.Lpgm
    .org 0x200
.Lhalt_psw:
    .quad 0x0002000000000000,0
.Lpgm:
    lpswe 0x150
.Lentry:
    xgr 1,1
    llilf 2,0x7fffffff
    mvc 0(1,1),0(2)
    xr 2,2
    lpswe .Lhalt_psw
