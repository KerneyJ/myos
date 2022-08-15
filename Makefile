AS=./cross/bin/i686-elf-as
CC=./cross/bin/i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
GRUB=grub-mkrescue

all: build-iso clean

run:
	qemu-system-i386 -cdrom myos.iso

.PHONY: build-iso
build-iso: myos.bin
	cp $^ isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB) -o myos.iso isodir

myos.bin: kernel.o boot.o
	$(CC) -o $@ $(LDFLAGS)

kernel.o: kernel.c
	$(CC) -c $^ $(CFLAGS) -o $@

boot.o: boot.s
	$(AS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf kernel.o
	rm -rf boot.o
	rm -rf myos.bin
