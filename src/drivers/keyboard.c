#include <stdint.h>
#include "supernova.h"

void keyboard_handler_c() {
    // Read the scancode from the keyboard's data port
    uint8_t scancode = inb(0x60);

    // If the top bit is set, it means a key was released. 
    // We only care about key presses (top bit clear).
    if (!(scancode & 0x80)) {
        kprint("KEY PRESSED!", 5, 0x0B); // Cyan text at row 5
    }

    // Tell the PIC we have finished handling the interrupt
    // This is the "Acknowledgment" (EOI - End of Interrupt)
    outb(0x20, 0x20);
}
