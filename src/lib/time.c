#include <stdint.h>
#include "supernova.h"

volatile uint64_t timer_ticks = 0;

void timer_init() {
    uint32_t divisor = 1193182 / 10; // 1000Hz (1ms)

    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void timer_handler() {
    timer_ticks++;
}

void msleep(uint32_t ms) {
    uint64_t end = timer_ticks + ms;
    while (timer_ticks < end) {
        __asm__ volatile("hlt");
    }
}
