KERNELDIR=kernel/kernel
INCLUDEDIR=kernel/include
LIBDIR=kernel/lib

CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
GRUB=grub-mkrescue

ifeq ($(ARCH), i386)
	ARCHDIR=kernel/arch/i386
	AS=cross/bin/i686-elf-as
	CC=cross/bin/i686-elf-gcc
	LD=cross/bin/i686-elf-ld
	EMU=qemu-system-i386 # if PVH notes error then try flag -machine type=pc-i440fx-3.1
endif

ifeq ($(ARCH), x86_64)
	ARCHDIR=kernel/arch/x86_64
	AS=cross/bin/x86_64-elf-as
	CC=cross/bin/x86_64-elf-gcc
	LD=cross/bin/x86_64-elf-ld
	EMU=qemu-system-x86_64 # -nographic -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 # added flag -machine q35 to fix Error loading uncompressed kernel without PVH ELF Note
endif

#QEMUOPTS = -machine virt -bios none -m 128M -smp 8 -nographic
#QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
#QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

include $(KERNELDIR)/make.config
include $(ARCHDIR)/make.config
include $(LIBDIR)/make.config

OBJS=$(KERNEL_ARCH_OBJS)\
	 $(KERNEL_OBJS) \
	 $(LIB_OBJS) \

all: build-iso

run-debug:
	$(EMU) -cdrom myos.iso -s -S &# -monitor stdio
	gdb myos.bin

run:
	$(EMU) -cdrom myos.iso

.PHONY: build-iso clean

build-iso: myos.bin
	cp $^ isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB) -o myos.iso isodir

myos.bin: $(OBJS)
	$(CC) -o $@ -T $(ARCHDIR)/linker.ld $(LDFLAGS) $(OBJS)

%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@ -isystem $(INCLUDEDIR) -D CONFIG_$(ARCH)

%.o: %.S
	$(CC) -c $^ -o $@ -isystem $(INCLUDEDIR)

%.o: %.s
	$(AS) -c $^ -o $@

clean:
	rm kernel/*/*.o
	rm kernel/*/*/*.o
	rm myos.*
