section .multiboot
align 4
    dd 0x1BADB002              ; multiboot number
    dd 0x00                    ; flags
    dd -(0x1BADB002 + 0x00)    ; checksum

; --- 32-bit Bootstrap Section ---
bits 32
section .text
global _start
extern kernel_main_64
extern gp              ; From gdt.c

_start:
    mov esp, stack_top

    ; 1. Setup Paging
    call setup_page_tables
    call enable_paging

    ; 2. Load 64-bit GDT
    lgdt [gdt64_ptr]

    ; 3. The "Hyperspace" Jump to 64-bit
    jmp 0x08:long_mode_start

; --- Paging Functions ---
setup_page_tables:
    mov eax, p3_table
    or eax, 0b11
    mov [p4_table], eax

    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax

    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_p2_table
    ret

enable_paging:
    mov eax, p4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret

; --- 64-bit Landing Zone ---
bits 64
long_mode_start:
    ; Load data segments with 0 (null) for 64-bit mode
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    
    call kernel_main_64
    hlt

; --- Data Section ---
section .rodata
align 8
gdt64:
    dq 0 ; null
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
gdt64_ptr:
    dw $ - gdt64 - 1
    dq gdt64

section .bss
align 4096
p4_table: resb 4096
p3_table: resb 4096
p2_table: resb 4096
stack_bottom: resb 16384
stack_top:
