CC = gcc
AS = nasm
LD = ld

# Only 64-bit flags needed now
CFLAGS64 = -m64 -ffreestanding -fno-stack-protector -fno-pie -fno-pic -nostdlib -mcmodel=large
ASFLAGS = -f elf64
LDFLAGS = -m elf_x86_64 -T linker.ld -no-pie -n

# We only need boot.o and our 64-bit kernel
OBJS = src/boot.o src/kernel64.o

all: supernova.bin

src/boot.o: src/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

src/kernel64.o: src/kernel64.c
	$(CC) $(CFLAGS64) -c $< -o $@

supernova.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f src/*.o supernova.bin supernova.iso
	rm -rf iso_root
