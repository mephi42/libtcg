    make configure-qemu configure-llvm
    make build-qemu build-llvm -j8
    make
    build/bin2llvm linux/arch/s390/boot/image
