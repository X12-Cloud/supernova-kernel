#ifndef SUPERNOVA_H
#define SUPERNOVA_H

#include <stdint.h>
#include <stddef.h>
#include "shell.h"

// important vars
extern int cursor_x;
extern int cursor_y;
extern int input_start_x;

// CPU Logic
void init_idt();
void pic_remap();

// I/O Helpers
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint16_t outw(uint16_t port, uint16_t data);

// Video/VGA
void kprint(const char* str, int row, uint8_t color);
void clear_screen();
void putchar(char c, uint8_t color);
void move_cursor_relative(int dx, int dy);
void update_cursor();
char get_char_at(int x, int y);
void print_hex_byte(uint8_t byte);

// Disk/ATA
void ata_wait_ready();
void ata_read_sector(uint32_t lba, uint16_t* buffer);
void ata_write_sector(uint32_t lba, uint16_t* buffer);
void fat32_init();
uint32_t cluster_to_sector(uint32_t cluster);
void format_to_83(char* input, char* output);

// Strings
int strcmp(const char* s1, const char* s2);
int str_to_int(char* s);
void kprint_int(uint32_t n, int color);
int memcmp(const void* s1, const void* s2, size_t n);

// Power Options
void reboot();

// System info
void get_cpu_vendor(char* vendor);
void get_cpu_brand(char* brand);

#endif
