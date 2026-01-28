# Supernova Kerne;
The x86_64 kernel that boots before you can even blink.

Supernova is a kernel built from the ground up for speed, exploration, and the pure joy of bare-metal programming. No bloat, no legacy junk—just raw code hitting the hardware.

## "Wait, did it just boot?"
Yes. Because the kernel is currently under 1MB, Supernova boots instantly.

While modern operating systems are busy loading splash screens, initializing thousands of subsystems, and phone-homing telemetry, Supernova is already at the prompt waiting for your next move. It is essentially the "Superleggera" of the software world.

# Current Features

- Instant Shell: A functional CLI that starts immediately.

- Finger Memory: Full keyboard history support with Up/Down arrow keys (so you don't have to re-type your sector reads).

- Hard Talk: Custom drivers for the Keyboard (Shift/Caps support) and VGA text mode.

- Storage: ATA PIO driver with FAT32 filesystem support.

- Tools: Built-in fetch for system info, hex for deep-diving sectors, and ls/cat for disk navigation.

- Manual Scrolling: Custom-built VGA scrolling logic that doesn't rely on BIOS calls.

# Build & Run
- If you have gcc and qemu, you’re ready to launch.
```bash
./run.sh -clean
```

## Build the kernel:
```bash
./run.sh -build
```

## Run the kernel
```bash
./run.sh -run
```

# What's Cooking?

- ELF Loader: Moving away from a monolithic kernel. Soon, we'll be loading and running standalone binaries straight off the FAT32 disk.

- Memory Management: Paging and virtual memory allocation.

- Multitasking: Teaching the CPU how to juggle multiple tasks.

Built for the x86_64 architecture. Speed is the only priority.
