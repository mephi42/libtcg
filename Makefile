include env

PKGS=glib-2.0 pixman-1 zlib
CFLAGS=\
	-isystem build/ \
	-isystem build/$(TARGET)-softmmu \
	-isystem src/ \
	-isystem src/include \
	-isystem src/target/$(TARGET) \
	-isystem src/tcg \
	-isystem src/tcg/$(HOST) \
	$$(pkg-config --cflags $(PKGS)) \
	-DNEED_CPU_H \
	-g \
	-Wall \
	-Wextra \
	-Werror
LDFLAGS=$$(pkg-config --libs $(PKGS)) -framework IOKit

TCG_GEN=build/tcg-gen

all: $(TCG_GEN)

clean:
		rm -f $(TCG_GEN)

TCG_GEN_SRC=\
	tcg-gen.c \
	$(shell find build \
		-name "*.o" \
		-a -not -path "build/hw/pci/pci-stub.o" \
		-a -not -path "build/qemu-img.o" \
		-a -not -path "build/qemu-io.o" \
		-a -not -path "build/qemu-nbd.o" \
		-a -not -path "build/qga/*.o" \
		-a -not -path "build/stubs/*.o" \
		-a -not -path "build/vl.o") \
	build/stubs/qmp_memory_device.o \
	build/stubs/target-get-monitor-def.o \
	build/stubs/target-monitor-defs.o \
	build/stubs/vmgenid.o \
	build/stubs/xen-hvm.o

$(TCG_GEN): $(TCG_GEN_SRC)
		gcc $(CFLAGS) $(LDFLAGS) $(TCG_GEN_SRC) -o $(TCG_GEN)

.PHONY: configure-qemu
configure-qemu:
		mkdir -p build
		cd build && ../src/configure \
			--target-list=$(TARGET)-softmmu \
			--disable-fdt \
			--audio-drv-list= \
			--disable-cocoa \
			--disable-vnc \
			--disable-nettle \
			--disable-gnutls \
			--disable-gcrypt \
			--disable-gtk \
			--disable-vte \
			--disable-usb-redir \
			--disable-curl \
			--disable-bzip2 \
			--disable-lzo

.PHONY: build-qemu
build-qemu:
		cd build && $(MAKE)
		gobjcopy --strip-symbol _main build/vl.o build/vl_nomain.o

.PHONY: project
project:
		./ls-files >libtcg.files
		(echo 'build' && \
			echo 'build/$(TARGET)-softmmu' && \
			echo 'src/' && \
			echo 'src/include' && \
			echo 'src/linux-user/$(TARGET)' && \
			echo 'src/target/$(TARGET)' && \
			echo 'src/tcg' && \
			echo 'src/tcg/$(HOST)') >libtcg.includes
