#include <stdint.h>
#include "supernova.h"

// Use uint16_t because each "pixel" is 2 bytes (char + color)
uint16_t* vga_buffer = (uint16_t*)0xB8000;
int cursor_x = 0;
int cursor_y = 0;
int input_start_x = 0;

char get_char_at(int x, int y) {
    volatile char* vidmem = (volatile char*)0xB8000;
    int offset = (y * 80 + x) * 2;
    return vidmem[offset];
}

void clear_screen() {
    volatile char* vidmem = (volatile char*)0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void putchar(char c, uint8_t color) {
    volatile char* vidmem = (volatile char*)0xB8000;

    if (cursor_y >= 25) { scroll_up(); }
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } 
    else if (c == '\b') {
        // Combined logic: Only backspace if we are past the prompt
        if (cursor_x > input_start_x) { 
            cursor_x--;
            int offset = (cursor_y * 80 + cursor_x) * 2;
            vidmem[offset] = ' ';
            vidmem[offset + 1] = color;
        }
    } 
    else {
        int offset = (cursor_y * 80 + cursor_x) * 2;
        vidmem[offset] = c;
        vidmem[offset + 1] = color;
        cursor_x++;
    }

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    update_cursor();
}

void move_cursor_relative(int dx, int dy) {
    int new_x = cursor_x + dx;
    int new_y = cursor_y + dy;

    // Boundary checks: stay on screen and stay after the prompt
    if (new_x >= input_start_x && new_x < 80) cursor_x = new_x;
    if (new_y >= 0 && new_y < 25) cursor_y = new_y;

    update_cursor();
}

void update_cursor() {
    uint16_t pos = cursor_y * 80 + cursor_x;
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
}

void kprint(const char* str, int row, uint8_t color) {
    if (row == -1) {
        for (int i = 0; str[i] != '\0'; i++) {
            putchar(str[i], color);
        }
    } else {
        volatile char* vidmem = (volatile char*)0xB8000 + (row * 80 * 2);
        for (int i = 0; str[i] != '\0'; i++) {
            vidmem[i*2] = str[i];
            vidmem[i*2 + 1] = color;
        }
    }
}

void print_hex_byte(uint8_t byte) {
    char* hex_chars = "0123456789ABCDEF";
    putchar(hex_chars[(byte >> 4) & 0x0F], 0x0F); // High nibble
    putchar(hex_chars[byte & 0x0F], 0x0F);        // Low nibble
}

void scroll_up() {
    // Move every line up by one
    // Each line is 80 words (160 bytes)
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 80; x++) {
            vga_buffer[y * 80 + x] = vga_buffer[(y + 1) * 80 + x];
        }
    }

    // Clear the bottom line
    for (int x = 0; x < 80; x++) {
        vga_buffer[24 * 80 + x] = (uint16_t)((0x07 << 8) | ' ');
    }

    cursor_y = 24;
}
