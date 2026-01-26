#ifndef SUPERNOVA_H
#define SUPERNOVA_H

#include <stdint.h>
#include <stddef.h>
#include "shell.h"

// important vars
extern int cursor_x;
extern int cursor_y;
extern int input_start_x;
extern volatile uint64_t timer_ticks;

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
char get_char_at(int x, int y);

// Strings
int strcmp(const char* s1, const char* s2);
void* memset(void* s, int c, size_t n); 

// Power Options
void reboot();
int reboot_delayed(int interval);

// Time
void timer_init();
void timer_handler();
void msleep(uint32_t ms); 

#endif
