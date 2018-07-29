    git submodule init
    git submodule update
    make deps -j$(getconf _NPROCESSORS_ONLN)
    make all test -j$(getconf _NPROCESSORS_ONLN)
    build/$(uname -s)-$(uname -m)/bin2llvm image image.bc
