    make configure-qemu
    make build-qemu -j8
    make
    build/tcg-gen linux/arch/s390/boot/image
