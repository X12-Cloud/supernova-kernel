#include <stdint.h>
#include "supernova.h"

void kernel_main_64() {
    clear_screen();
    init_idt();
    //kprint("Supernova 64-bit: Keyboard Listening...", 0, 0x0E);
    shell_exec();
    while(1);
}
