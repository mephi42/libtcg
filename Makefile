TOP:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SHELL:=/bin/bash
include env
-include modules/Makefile.include

PKGS=glib-2.0 pixman-1 zlib
LLVM_COMPONENTS=analysis bitwriter core
LLVM_TARGETS=SystemZ;X86
OS=$(shell uname -s)
UNAME=$(OS)-$(shell uname -m)
QEMU=$(TOP)/qemu
QEMU_BUILD=$(TOP)/qemu-build/$(UNAME)
QEMU_KVM_BUILD=$(TOP)/qemu-kvm-build/$(UNAME)
LLVM=$(TOP)/llvm
LLVM_BUILD=$(TOP)/llvm-build/$(UNAME)
GDB=$(TOP)/binutils-gdb
GDB_BUILD=$(TOP)/gdb-build/$(UNAME)
SRC=$(TOP)/src
BUILD=$(TOP)/build/$(UNAME)
OBJCOPY?=$(shell which gobjcopy 2>/dev/null || which objcopy)
TRIPLE=s390x-ibm-linux-gnu
TRIPLE2=s390x-linux-gnu
CROSS_GCC=$(shell which $(TRIPLE)-gcc 2>/dev/null || which $(TRIPLE2)-gcc)
CROSS_LD=$(shell which $(TRIPLE)-ld 2>/dev/null || which $(TRIPLE2)-ld)
CROSS_OBJDUMP=$(shell which $(TRIPLE)-objdump 2>/dev/null || which $(TRIPLE2)-objdump)
CFLAGS=\
	-isystem $(QEMU_BUILD) \
	-isystem $(QEMU_BUILD)/$(TARGET)-softmmu \
	-iquote $(TOP)/include \
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
	-lpthread
ifeq ($(OS),Linux)
LDFLAGS+=-lutil
endif
ifeq ($(OS),Darwin)
LDFLAGS+=-framework IOKit
endif

BIN2LLVM=$(BUILD)/bin2llvm

RUNTIME_OBJECTS=\
	$(BUILD)/qemu/accel/tcg/tcg-runtime.bc \
	$(BUILD)/qemu/fpu/softfloat.bc \
	$(BUILD)/qemu/hw/intc/s390_flic.bc \
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
	$(BUILD)/src/runtime.bc \
	$(BUILD)/src/runtime-stubs.bc \
	$(MODULES_RUNTIME_OBJECTS)

all: $(BIN2LLVM) $(RUNTIME_OBJECTS)

deps: qemu llvm qemu-kvm gdb

BIN2LLVM_OWN_OBJS=\
	$(BUILD)/bin2llvm.o \
	$(BUILD)/libtcg.o \
	$(BUILD)/llvm-core-extras.o \
	$(BUILD)/tcg2llvm.o

clean:
		rm -f $(BIN2LLVM) $(RUNTIME_OBJECTS) $(BIN2LLVM_OWN_OBJS)

BIN2LLVM_EXCLUDE_OBJS=\
	-a -not -path "$(QEMU_BUILD)/contrib/*.o" \
	-a -not -path "$(QEMU_BUILD)/hw/pci/pci-stub.o" \
	-a -not -path "$(QEMU_BUILD)/qemu-bridge-helper.o" \
	-a -not -path "$(QEMU_BUILD)/qemu-img.o" \
	-a -not -path "$(QEMU_BUILD)/qemu-io.o" \
	-a -not -path "$(QEMU_BUILD)/qemu-nbd.o" \
	-a -not -path "$(QEMU_BUILD)/qga/*.o" \
	-a -not -path "$(QEMU_BUILD)/scsi/qemu-pr-helper.o" \
	-a -not -path "$(QEMU_BUILD)/stubs/*.o" \
	-a -not -path "$(QEMU_BUILD)/tests/*.o" \
	-a -not -path "$(QEMU_BUILD)/$(TARGET)-softmmu/tcg/tcg.o" \
	-a -not -path "$(QEMU_BUILD)/vl.o"
ifeq ($(OS),Linux)
BIN2LLVM_EXCLUDE_OBJS+=\
	-a -not -path "$(QEMU_BUILD)/hw/virtio/vhost-stub.o"
endif
BIN2LLVM_OBJS=\
	$(BIN2LLVM_OWN_OBJS) \
	$(shell find $(QEMU_BUILD) -name "*.o" $(BIN2LLVM_EXCLUDE_OBJS)) \
	$(QEMU_BUILD)/stubs/qmp_memory_device.o \
	$(QEMU_BUILD)/stubs/slirp.o \
	$(QEMU_BUILD)/stubs/target-get-monitor-def.o \
	$(QEMU_BUILD)/stubs/target-monitor-defs.o \
	$(QEMU_BUILD)/stubs/tpm.o \
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
		clang -c -emit-llvm $(CFLAGS_RUNTIME) -I$(QEMU_BUILD)/$(shell dirname $< | cut -d/ -f2-) $< -o $@

$(BIN2LLVM): $(BIN2LLVM_OBJS)
		mkdir -p $(shell dirname $@)
		$(CXX) -o $(BIN2LLVM) $(BIN2LLVM_OBJS) $(LDFLAGS)

.PRECIOUS: $(BUILD)/test/%-code.o
$(BUILD)/test/%-code.o: test/%.S $(wildcard test/*.h)
		mkdir -p $(shell dirname $@)
		$(CROSS_GCC) -c -march=zEC12 -o $@ $<

.PRECIOUS: $(BUILD)/test/%.bin
$(BUILD)/test/%.bin: $(BUILD)/test/%-code.o bin/qemu-system-$(TARGET)-trace $(QEMU_BUILD)/$(TARGET)-softmmu/qemu-system-$(TARGET)
		$(CROSS_LD) -o $@.tmp -Ttext=0 --oformat=binary $<
		$(CROSS_OBJDUMP) -b binary -m s390 -D $@.tmp >$(BUILD)/test/$*.txt
		bin/qemu-system-$(TARGET)-trace -kernel $@.tmp -D $(BUILD)/test/$*.log
		tail -9 <$(BUILD)/test/$*.log | head -1 | grep R02=0000000000000000 >/dev/null
		mv $@.tmp $@

.PRECIOUS: $(BUILD)/test/%.bc
$(BUILD)/test/%.bc: $(BUILD)/test/%.bin $(BIN2LLVM)
		$(BIN2LLVM) $< $@
		$(LLVM_BUILD)/bin/llvm-dis -o $(BUILD)/test/$*.ll $@

.PRECIOUS: $(BUILD)/test/%.o
$(BUILD)/test/%.o: $(BUILD)/test/%.bc
		clang -c -o $@ $<

.PRECIOUS: $(BUILD)/test/%
$(BUILD)/test/%: $(BUILD)/test/%.o $(RUNTIME_OBJECTS)
		clang -o $@ $< $(RUNTIME_OBJECTS) $(shell pkg-config --libs glib-2.0)

.PHONY: check-bin-%
check-bin-%: $(BUILD)/test/%
		[[ $@ = check-bin-qemu-* ]] || $<

.PHONY: check
check: $(foreach s,$(wildcard test/*.S),check-bin-$(basename $(notdir $(s))))

QEMU_KVM_ARGS=-enable-kvm -smp 1 -m 4096 -nographic -nodefaults -gdb stdio -S

.PHONY: check-kvm-%
check-kvm-%: $(BUILD)/test/%.bin
		bin/gdb \
			-batch \
			-ex "target remote | $(QEMU_KVM_BUILD)/$(TARGET)-softmmu/qemu-system-$(TARGET) $(QEMU_KVM_ARGS) -kernel $<" \
			-x share/gdb-insn-trace >$(BUILD)/test/$*-kvm.log
		tail -17 $(BUILD)/test/$*-kvm.log | head -1 | grep 'r2[[:space:]]\+0x0[[:space:]]\+0' >/dev/null

.PHONY: check-kvm
check-kvm: $(foreach s,$(wildcard test/*.S),check-kvm-$(basename $(notdir $(s))))

QEMU_CONFIG_COMMON=\
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
	--disable-lzo \
	--disable-vhost-vsock \
	--disable-virtfs \
	--disable-live-block-migration \
	--disable-vhost-net \
	--disable-vhost-crypto \
	--disable-vhost-scsi \
	--disable-libusb \
	--disable-curses \
	--disable-xen \
	--disable-brlapi \
	--disable-bluez \
	--disable-docs \
	--disable-cap-ng \
	--disable-vhost-user \
	--disable-spice \
	--disable-rbd \
	--disable-xfsctl \
	--disable-libiscsi \
	--disable-guest-agent \
	--disable-tpm \
	--disable-replication \
	--disable-slirp \
	--disable-sdl \
	--disable-linux-aio \
	--disable-numa \
	--disable-libnfs \
	--disable-libssh2 \
	--disable-glusterfs \
	--disable-vde

QEMU_CONFIG=\
	$(QEMU_CONFIG_COMMON) \
	--cpu=unknown \
	--enable-tcg-interpreter \
	--disable-kvm

.PHONY: configure-qemu
configure-qemu $(QEMU_BUILD)/Makefile:
		mkdir -p $(QEMU_BUILD)
		cd $(QEMU_BUILD) && $(QEMU)/configure $(QEMU_CONFIG)

ifeq ($(OS),Darwin)
SYMBOL_PREFIX=_
endif

.PHONY: qemu
qemu: $(QEMU_BUILD)/Makefile
		cd $(QEMU_BUILD) && $(MAKE)
		$(OBJCOPY) --strip-symbol $(SYMBOL_PREFIX)main $(QEMU_BUILD)/vl.o $(QEMU_BUILD)/vl_nomain.o
		$(OBJCOPY) --globalize-symbol $(SYMBOL_PREFIX)helper_table $(QEMU_BUILD)/$(TARGET)-softmmu/tcg/tcg.o $(QEMU_BUILD)/$(TARGET)-softmmu/tcg/tcg_with_helper_table.o

.PHONY: configure-llvm
configure-llvm $(LLVM_BUILD)/Makefile:
		mkdir -p $(LLVM_BUILD)
		cd $(LLVM_BUILD) && cmake -DLLVM_ENABLE_TERMINFO=off -DLLVM_TARGETS_TO_BUILD="$(LLVM_TARGETS)" $(LLVM)

.PHONY: llvm
llvm: $(LLVM_BUILD)/Makefile
		cd $(LLVM_BUILD) && $(MAKE)

QEMU_KVM_CONFIG=\
	$(QEMU_CONFIG_COMMON) \
	--enable-kvm

.PHONY: configure-qemu-kvm
configure-qemu-kvm $(QEMU_KVM_BUILD)/Makefile:
		mkdir -p $(QEMU_KVM_BUILD)
		cd $(QEMU_KVM_BUILD) && $(QEMU)/configure $(QEMU_KVM_CONFIG)

.PHONY: qemu-kvm
qemu-kvm: $(QEMU_KVM_BUILD)/Makefile
		cd $(QEMU_KVM_BUILD) && $(MAKE)

GDB_CONFIG=\
	--target=$(TRIPLE) \
	--without-guile

.PHONY: configure-gdb
configure-gdb $(GDB_BUILD)/Makefile:
		mkdir -p $(GDB_BUILD)
		cd $(GDB_BUILD) && $(GDB)/configure $(GDB_CONFIG)

.PHONY: gdb
gdb: $(GDB_BUILD)/Makefile
		cd $(GDB_BUILD) && $(MAKE) all-gdb

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
			echo '$(TOP)/include' && \
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
