#include <stdint.h>

void kprint(const char* str, int row, uint8_t color);
void init_idt();

void kernel_main_64() {
    init_idt();
    kprint("Supernova 64-bit: Keyboard Listening...", 0, 0x0E);
    while(1);
}
