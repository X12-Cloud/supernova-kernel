#include <stdint.h>
#include <stddef.h>
#include "supernova.h"

// Forward declarations
void pic_remap();
extern void timer_irq_asm();
extern void keyboard_handler_asm();
extern void exception_handler_asm();
extern void irq_dummy_handler_asm();
extern void idt_load(uint64_t);

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;     
    uint8_t  ist;          
    uint8_t  flags;        
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Aligning to 16 bytes is crucial for x86_64 interrupts
struct idt_entry idt[256] __attribute__((aligned(16)));
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt[num].base_mid  = (uint16_t)((base >> 16) & 0xFFFF);
    idt[num].base_high = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    idt[num].selector  = sel;
    idt[num].ist       = 0;
    idt[num].flags     = flags;
    idt[num].reserved  = 0;
}

void init_idt() {
    idtp.limit = (uint16_t)(sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    memset(&idt, 0, sizeof(idt));

    // Point everything to exception handler first (0-255)
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint64_t)exception_handler_asm, 0x08, 0x8E);
    }

    // Remap PIC but keep them masked for now
    pic_remap();

    // Specific gates for Hardware
    idt_set_gate(32, (uint64_t)timer_irq_asm, 0x08, 0x8E);
    idt_set_gate(33, (uint64_t)keyboard_handler_asm, 0x08, 0x8E);
    
    // This is the "Point of No Return"
    idt_load((uint64_t)&idtp);

    // If you see this, the IDT loaded without crashing!
    kprint("IDT loaded successfully.\n", -1, 0x0A);
}

void pic_remap() {
    outb(0x20, 0x11); 
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFC); 
    outb(0xA1, 0xFF);
}
