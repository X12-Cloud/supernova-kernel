CC = gcc
AS = nasm
LD = ld

CFLAGS = -m64 -ffreestanding -fno-stack-protector -fno-pie -fno-pic -nostdlib -mcmodel=large -Iinclude
ASFLAGS = -f elf64
LDFLAGS = -m elf_x86_64 -T linker.ld -no-pie -n

# 1. Collect ALL source files
C_SOURCES = $(shell find src -name "*.c")
ASM_SOURCES = $(shell find src -name "*.asm")

# 2. Convert ALL of them to .o files (one-to-one)
OBJS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

all: supernova.bin

# 3. The ONLY rule for the final binary
supernova.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# 4. Rule for compiling C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 5. Rule for compiling ALL assembly files
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(shell find src -name "*.o") supernova.bin supernova.iso
	rm -rf iso_root
