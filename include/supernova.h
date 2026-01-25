#ifndef SUPERNOVA_H
#define SUPERNOVA_H

#include <stdint.h>

// CPU Logic
void init_idt();
void pic_remap();

// I/O Helpers
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

// Video/VGA
void kprint(const char* str, int row, uint8_t color);
void clear_screen();
void putchar(char c, uint8_t color);
void move_cursor_relative(int dx, int dy);
void update_cursor();

#endif
