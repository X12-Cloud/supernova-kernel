#include "supernova.h"

void kernel_main_64() {
    timer_init();      // Setup PIT frequency
    init_idt();        // Load IDT and Unmask PIC
    
    __asm__ volatile("sti"); 
    
    shell_exec(); 
}
