KERNELDIR=kernel/kernel
ARCHDIR=kernel/arch/i386
INCLUDEDIR=kernel/include
LIBDIR=kernel/lib

AS=cross/bin/i686-elf-as
CC=cross/bin/i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc
GRUB=grub-mkrescue

include $(KERNELDIR)/make.config
include $(ARCHDIR)/make.config
include $(LIBDIR)/make.config

OBJS=$(KERNEL_ARCH_OBJS)\
	 $(KERNEL_OBJS) \
	 $(LIB_OBJS) \

all: build-iso

run:
	qemu-system-i386 -kernel myos.bin

.PHONY: build-iso clean

build-iso: myos.bin
	cp $^ isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB) -o myos.iso isodir

myos.bin: $(OBJS)
	$(CC) -o $@ -T $(ARCHDIR)/linker.ld $(LDFLAGS) $(OBJS)

%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@ -isystem $(INCLUDEDIR)

%.o: %.s
	$(AS) -c $^ -o $@

clean:
	rm kernel/*/*.o
	rm kernel/*/*/*.o
	rm myos.*
