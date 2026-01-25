#include "supernova.h"

unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};

void keyboard_handler_c() {
    uint8_t scancode = inb(0x60);

    if (!(scancode & 0x80)) {
        switch(scancode) {
            case 0x48: move_cursor_relative(0, -1); break; // UP
            case 0x50: move_cursor_relative(0, 1);  break; // DOWN
            case 0x4B: move_cursor_relative(-1, 0); break; // LEFT
            case 0x4D: move_cursor_relative(1, 0);  break; // RIGHT
            default:
                char c = kbd_us[scancode];
                if (c > 0) putchar(c, 0x0F);
                break;
        }
    }
    outb(0x20, 0x20);
}
