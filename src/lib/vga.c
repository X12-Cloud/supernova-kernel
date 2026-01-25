#include <stdint.h>

void clear_screen() {
    volatile char* vidmem = (volatile char*)0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = 0x07;
    }
}

void kprint(const char* str, int row, uint8_t color) {
    volatile char* vidmem = (volatile char*)0xB8000 + (row * 80 * 2);
    for (int i = 0; str[i] != '\0'; i++) {
        vidmem[i*2] = str[i];
        vidmem[i*2 + 1] = color;
    }
}
