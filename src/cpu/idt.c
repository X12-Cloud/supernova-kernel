#include <stdint.h>
// Forward declarations so the compiler doesn't panic
void pic_remap();
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

// IDT Entry structure for 64-bit mode
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;     // Usually 0x08 (Code Segment)
    uint8_t  ist;          // Interrupt Stack Table (set to 0 for now)
    uint8_t  flags;        // Type and attributes (0x8E for interrupt gate)
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

// Function to load the IDT (defined in assembly later)
extern void idt_load(uint64_t);

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_mid  = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector  = sel;
    idt[num].ist       = 0;
    idt[num].flags     = flags;
    idt[num].reserved  = 0;
}

extern void dummy_handler_asm();
extern void keyboard_handler_asm();

void init_idt() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    // 1. Point EVERY interrupt to the dummy handler
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint64_t)dummy_handler_asm, 0x08, 0x8E);
    }

    // 2. Remap the PIC (moves IRQs to 32-47)
    pic_remap();

    // 3. Set the Keyboard specifically to its real handler
    idt_set_gate(33, (uint64_t)keyboard_handler_asm, 0x08, 0x8E);

    // 4. Load the IDT
    idt_load((uint64_t)&idtp);

    // 5. FINALLY enable interrupts
    __asm__ volatile ("sti");
}

void pic_remap() {
    uint8_t a1, a2;

    // Save current masks
    a1 = inb(0x21);
    a2 = inb(0xA1);

    // Start initialization sequence
    outb(0x20, 0x11); 
    outb(0xA0, 0x11);

    // Map Master PIC to 0x20 (Interrupt 32)
    // Map Slave PIC to 0x28 (Interrupt 40)
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    // Tell them how they are connected
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // Set Environment mode
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Restore masks
    outb(0x21, a1);
    outb(0xA1, a2);
}
