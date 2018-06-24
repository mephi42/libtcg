include env
-include modules/Makefile.include

PKGS=glib-2.0 pixman-1 zlib
LLVM_COMPONENTS=analysis bitwriter core
QEMU=qemu
QEMU_BUILD=qemu-build
LLVM=llvm
LLVM_BUILD=llvm-build
SRC=src
BUILD=build
OBJCOPY?=gobjcopy
CFLAGS=\
	-isystem $(QEMU_BUILD) \
	-isystem $(QEMU_BUILD)/$(TARGET)-softmmu \
	-iquote include \
	-isystem $(QEMU) \
	-isystem $(QEMU)/accel/tcg \
	-isystem $(QEMU)/include \
	-isystem $(QEMU)/target/$(TARGET) \
	-isystem $(QEMU)/tcg \
	-isystem $(QEMU)/tcg/$(HOST) \
	-isystem $(LLVM)/include \
	-isystem $(LLVM_BUILD)/include \
	$(shell pkg-config --cflags $(PKGS)) \
	-DNEED_CPU_H \
	-g \
	-Wall \
	-Wextra \
	-Werror
CXXFLAGS=$(CFLAGS) -std=c++11
LDFLAGS=\
	$(shell $(LLVM_BUILD)/bin/llvm-config --ldflags --libs $(LLVM_COMPONENTS)) \
	$(shell pkg-config --libs $(PKGS)) \
	-framework IOKit

BIN2LLVM=$(BUILD)/bin2llvm

RUNTIME_OBJECTS=\
	$(BUILD)/qemu/fpu/softfloat.bc \
	$(BUILD)/src/runtime.bc \
	$(BUILD)/src/runtime-stubs.bc \
	$(BUILD)/qemu/target/s390x/cc_helper.bc \
	$(BUILD)/qemu/target/s390x/crypto_helper.bc \
	$(BUILD)/qemu/target/s390x/fpu_helper.bc \
	$(BUILD)/qemu/target/s390x/excp_helper.bc \
	$(BUILD)/qemu/target/s390x/int_helper.bc \
	$(BUILD)/qemu/target/s390x/interrupt.bc \
	$(BUILD)/qemu/target/s390x/helper.bc \
	$(BUILD)/qemu/target/s390x/mem_helper.bc \
	$(BUILD)/qemu/target/s390x/misc_helper.bc \
	$(BUILD)/qemu/tcg/tcg-common.bc \
	$(BUILD)/qemu/util/log.bc \
	$(MODULES_RUNTIME_OBJECTS)

all: $(BIN2LLVM) $(RUNTIME_OBJECTS)

clean:
		rm -f $(BIN2LLVM)

BIN2LLVM_OBJS=\
	$(BUILD)/bin2llvm.o \
	$(BUILD)/libtcg.o \
	$(BUILD)/llvm-core-extras.o \
	$(BUILD)/tcg2llvm.o \
	$(shell find $(QEMU_BUILD) \
		-name "*.o" \
		-a -not -path "$(QEMU_BUILD)/hw/pci/pci-stub.o" \
		-a -not -path "$(QEMU_BUILD)/qemu-img.o" \
		-a -not -path "$(QEMU_BUILD)/qemu-io.o" \
		-a -not -path "$(QEMU_BUILD)/qemu-nbd.o" \
		-a -not -path "$(QEMU_BUILD)/qga/*.o" \
		-a -not -path "$(QEMU_BUILD)/stubs/*.o" \
		-a -not -path "$(QEMU_BUILD)/$(TARGET)-softmmu/tcg/tcg.o" \
		-a -not -path "$(QEMU_BUILD)/vl.o") \
	$(QEMU_BUILD)/stubs/qmp_memory_device.o \
	$(QEMU_BUILD)/stubs/target-get-monitor-def.o \
	$(QEMU_BUILD)/stubs/target-monitor-defs.o \
	$(QEMU_BUILD)/stubs/vmgenid.o \
	$(QEMU_BUILD)/stubs/xen-hvm.o

$(BUILD)/%.o: $(SRC)/%.c $(wildcard include/*.h)
		mkdir -p $(shell dirname $@)
		$(CC) -c $(CFLAGS) $< -o $@

$(BUILD)/%.o: $(SRC)/%.cpp $(wildcard include/*.h)
		mkdir -p $(shell dirname $@)
		$(CXX) -c $(CXXFLAGS) $< -o $@

CFLAGS_RUNTIME=\
	$(CFLAGS) \
	-Wno-unused-parameter \
	-Wno-sign-compare \
	-Wno-missing-field-initializers

$(BUILD)/%.bc: %.c $(wildcard include/*.h)
		mkdir -p $(shell dirname $@)
		clang -c -emit-llvm $(CFLAGS_RUNTIME) $< -o $@

$(BIN2LLVM): $(BIN2LLVM_OBJS)
		mkdir -p $(shell dirname $@)
		$(CXX) $(LDFLAGS) $(BIN2LLVM_OBJS) -o $(BIN2LLVM)

.PRECIOUS: build/test/%-code.o
build/test/%-code.o: test/%.s $(wildcard test/*.h)
		mkdir -p $(shell dirname $@)
		$(CPP) $< | s390x-ibm-linux-gnu-as -o $@

.PRECIOUS: build/test/%.bin
build/test/%.bin: build/test/%-code.o
		s390x-ibm-linux-gnu-ld -o $@.tmp -Ttext=0 --oformat=binary $<
		s390x-ibm-linux-gnu-objdump -b binary -m s390 -D $@.tmp >build/test/$*.txt
		bin/qemu-system-s390x-trace -kernel $@.tmp -D build/test/$*.log
		tail -9 <build/test/$*.log | head -1 | grep R02=0000000000000000 >/dev/null
		mv $@.tmp $@

.PRECIOUS: build/test/%.bc
build/test/%.bc: build/test/%.bin $(BIN2LLVM)
		$(BIN2LLVM) $< $@
		llvm-dis -o build/test/$*.ll $@

.PRECIOUS: build/test/%.o
build/test/%.o: build/test/%.bc
		clang -c -o $@ $<

.PRECIOUS: build/test/%
build/test/%: build/test/%.o $(RUNTIME_OBJECTS)
		clang -o $@ $< $(RUNTIME_OBJECTS) $(shell pkg-config --libs glib-2.0)

.PHONY: test-%
test-%: build/test/%
		$<

.PHONY: test
test: test-minimal test-pgm test-xc0 test-l test-st test-balr test-basr

.PHONY: configure-qemu
configure-qemu:
		mkdir -p $(QEMU_BUILD)
		cd $(QEMU_BUILD) && ../$(QEMU)/configure \
			--cpu=unknown \
			--enable-tcg-interpreter \
			--target-list=$(TARGET)-softmmu \
			--enable-debug \
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
		cd $(QEMU_BUILD) && $(MAKE)
		$(OBJCOPY) --strip-symbol _main $(QEMU_BUILD)/vl.o $(QEMU_BUILD)/vl_nomain.o
		$(OBJCOPY) --globalize-symbol _helper_table $(QEMU_BUILD)/$(TARGET)-softmmu/tcg/tcg.o $(QEMU_BUILD)/$(TARGET)-softmmu/tcg/tcg_with_helper_table.o

.PHONY: configure-llvm
configure-llvm:
		mkdir -p $(LLVM_BUILD)
		cd $(LLVM_BUILD) && cmake -DLLVM_ENABLE_TERMINFO=off -DLLVM_TARGETS_TO_BUILD= ../$(LLVM)

.PHONY: build-llvm
build-llvm:
		cd $(LLVM_BUILD) && $(MAKE)

.PHONY: project
project:
		(find . -type f \
			-a -not -path '*/.git/*' \
			-a -not -path '*/aarch64/*' \
			-a -not -path '*/alpha/*' \
			-a -not -path '*/arm/*' \
			-a -not -path '*/cris/*' \
			-a -not -path '*/i386/*' \
			-a -not -path '*/hppa/*' \
			-a -not -path '*/lm32/*' \
			-a -not -path '*/m68k/*' \
			-a -not -path '*/microblaze/*' \
			-a -not -path '*/mips/*' \
			-a -not -path '*/mips64/*' \
			-a -not -path '*/moxie/*' \
			-a -not -path '*/nios2/*' \
			-a -not -path '*/openrisc/*' \
			-a -not -path '*/ppc/*' \
			-a -not -path '*/ppc64/*' \
			-a -not -path '*/riscv/*' \
			-a -not -path '*/sh4/*' \
			-a -not -path '*/sparc/*' \
			-a -not -path '*/sparc64/*' \
			-a -not -path '*/tilegx/*' \
			-a -not -path '*/tricore/*' \
			-a -not -path '*/unicore32/*' \
			-a -not -path '*/xtensa/*' \
			-a -not -name '*.d' \
			-a -not -name '*.o' \
			-a -not -name '*.bc') >libtcg.files
		(echo '$(QEMU_BUILD)' && \
			echo '$(QEMU_BUILD)/$(TARGET)-softmmu' && \
			echo 'include' && \
			echo '$(QEMU)' && \
			echo '$(QEMU)/accel/tcg' && \
			echo '$(QEMU)/include' && \
			echo '$(QEMU)/linux-user/$(TARGET)' && \
			echo '$(QEMU)/target/$(TARGET)' && \
			echo '$(QEMU)/tcg' && \
			echo '$(QEMU)/tcg/$(HOST)' && \
			echo '$(LLVM)/include' && \
			echo '$(LLVM_BUILD)/include') >libtcg.includes
		(echo '#define NEED_CPU_H' && \
			echo '#define CONFIG_DEBUG_TCG' && \
			echo '#define CONFIG_TCG' && \
			echo '#define CONFIG_SOFTMMU') >libtcg.config
		echo '[General]' >libtcg.creator
