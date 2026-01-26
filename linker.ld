ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
    . = 1M;

    .text :
    {
        KEEP(*(.multiboot))    /* Force header to the top */
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data : ALIGN(4096) { *(.data) }
    .bss  : ALIGN(4096) { *(.bss) *(COMMON) }
}
