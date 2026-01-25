bits 64
global idt_load

idt_load:
    lidt [rdi]    ; RDI holds the first argument (idtp address) in 64-bit calling convention
    ret

[bits 64]
extern keyboard_handler_c
global keyboard_handler_asm

keyboard_handler_asm:
    push rax            ; Save all registers being used
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call keyboard_handler_c ; Jump to our C code!

    pop r11             ; Restore everything exactly as it was
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq               ; Special "Interrupt Return" for 64-bit

global dummy_handler_asm

dummy_handler_asm:
    push rax
    mov al, 0x20
    out 0x20, al    ; Send EOI to PIC so it stops yelling
    pop rax
    iretq
