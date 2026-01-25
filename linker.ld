ENTRY(_start)

SECTIONS
{
    /* Tell the linker to start as low as possible in the file */
    . = 1M;

    .text : {
        /* This MUST be first */
        *(.multiboot)
        *(.text)
    }

    /* Remove the BLOCK(4K) and ALIGN(4K) from everywhere for now */
    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(COMMON) *(.bss) }
}
