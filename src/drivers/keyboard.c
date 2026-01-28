#include "supernova.h"

unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};

unsigned char kbd_us_caps[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',   
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0
};

int shift_pressed = 0;
int caps_lock_on = 0;

void keyboard_handler_c() {
    uint8_t scancode = inb(0x60);

    // Handle capslock globally (toggle)
    if (scancode == 0x3A) {
        caps_lock_on = !caps_lock_on; // Flip it: 0 becomes 1, 1 becomes 0
    }

    // Handle Shift globally (both Press and Release)
    if (scancode == 0x2A) {
        shift_pressed = 1;
    } else if (scancode == 0xAA) {
        shift_pressed = 0;
    }

    // Handle Key Presses only (scancodes < 0x80)
    if (!(scancode & 0x80)) {
        switch(scancode) {
            case 0x48: move_cursor_relative(0, -1); break;
            case 0x50: move_cursor_relative(0, 1);  break;
            case 0x4B: move_cursor_relative(-1, 0); break;
            case 0x4D: move_cursor_relative(1, 0);  break;
            case 0x1C: 
                shell_handle_enter(); 
                break;
            case 0x2A: break; // Already handled above
            default: {
                // Pick the table based on shift_pressed/caps_lock_on
                char c;
                int use_caps = shift_pressed ^ caps_lock_on; 
		if (use_caps) {
    		    c = kbd_us_caps[scancode];
		} else {
    		    c = kbd_us[scancode];
		}

                if (c > 0) putchar(c, 0x0F);
                break;
            }
        }
    }

    // Send EOI (End of Interrupt) to the PIC
    outb(0x20, 0x20);
}
