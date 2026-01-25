bits 32                         ; Multiboot 1 is 32-bit
section .multiboot
align 4
    dd 0x1BADB002               ; Magic number
    dd 0x00                     ; Flags
    dd -(0x1BADB002 + 0x00)     ; Checksum

section .text
global _start
extern kernel_main              ; This is your C function

_start:
    cli                         ; Disable interrupts
    call kernel_main            ; Jump to C!
    hlt                         ; Halt if C returns
