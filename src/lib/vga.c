#include <stdint.h>
#include "supernova.h"

static int cursor_x = 0;
static int cursor_y = 0;

void clear_screen() {
    volatile char* vidmem = (volatile char*)0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = 0x07; // Light grey on black
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void putchar(char c, uint8_t color) {
    volatile char* vidmem = (volatile char*)0xB8000;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } 
    else if (c == '\b') {
        // Only move back if we aren't at the very start of the line
        if (cursor_x > 0) {
            cursor_x--;
            // Overwrite the old character with a space
            int offset = (cursor_y * 80 + cursor_x) * 2;
            vidmem[offset] = ' ';
            vidmem[offset + 1] = color;
        } 
        // Optional: If you want to go back to the previous line when x == 0
        /*
        else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = 79;
            // ... same erase logic ...
        }
        */
    } 
    else {
        int offset = (cursor_y * 80 + cursor_x) * 2;
        vidmem[offset] = c;
        vidmem[offset + 1] = color;
        cursor_x++;
    }

    // Wrap to next line if we hit the edge
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    update_cursor();
}

// Move cursor relative to current position
void move_cursor_relative(int dx, int dy) {
    int new_x = cursor_x + dx;
    int new_y = cursor_y + dy;

    // Boundary checks (Don't let the user leave the 80x25 box)
    if (new_x >= 0 && new_x < 80) cursor_x = new_x;
    if (new_y >= 0 && new_y < 25) cursor_y = new_y;

    update_cursor();
}

void update_cursor() {
    uint16_t pos = cursor_y * 80 + cursor_x;

    // Send the high byte of the cursor position to VGA index 0x0E
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    // Send the low byte to VGA index 0x0F
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
}

void kprint(const char* str, int row, uint8_t color) {
    volatile char* vidmem = (volatile char*)0xB8000 + (row * 80 * 2);
    for (int i = 0; str[i] != '\0'; i++) {
        vidmem[i*2] = str[i];
        vidmem[i*2 + 1] = color;
    }
}
