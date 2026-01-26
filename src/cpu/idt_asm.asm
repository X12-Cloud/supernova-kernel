[bits 64]
global idt_load
global keyboard_handler_asm
global timer_irq_asm
global irq_dummy_handler_asm
global exception_handler_asm

extern keyboard_handler_c
extern timer_handler

idt_load:
    lidt [rdi]
    ret

; Macro to save all state
%macro push_all 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro pop_all 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

timer_irq_asm:
    push_all          ; Save everything
    call timer_handler
    
    mov al, 0x20      ; End of Interrupt signal
    out 0x20, al      ; Send to Master PIC
    
    pop_all           ; Restore everything
    iretq             ; Return

keyboard_handler_asm:
    push_all
    call keyboard_handler_c
    
    mov al, 0x20      ; End of Interrupt signal
    out 0x20, al      ; Send to Master PIC
    
    pop_all
    iretq

irq_dummy_handler_asm:
    push rax
    mov al, 0x20
    out 0x20, al
    pop rax
    iretq

exception_handler_asm:
    ; THE PANIC BUTTON: Turns the screen Red to show a crash happened
    mov rdi, 0xB8000
    mov rcx, 80 * 25
.loop:
    mov word [rdi], 0x4F21 ; '!' on Red background
    add rdi, 2
    loop .loop
    cli
    hlt
    jmp $
