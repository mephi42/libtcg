    make configure-qemu configure-llvm
    make build-qemu build-llvm -j8
    make
    build/tcg-gen linux/arch/s390/boot/image
