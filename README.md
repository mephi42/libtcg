    git submodule init
    git submodule update
    make configure-qemu configure-llvm -j8
    make build-qemu build-llvm -j8
    make all test -j8
    build/$(uname -s)-$(uname -m)/bin2llvm image image.bc
