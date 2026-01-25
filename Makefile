CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pie -fno-pic -nostdlib
# Added -n to kill that 0x1000 padding
LDFLAGS = -m elf_i386 -T linker.ld -no-pie -n
ASFLAGS = -f elf32

OBJS = src/boot.o src/kernel.o

all: supernova.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

supernova.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f src/*.o supernova.bin
