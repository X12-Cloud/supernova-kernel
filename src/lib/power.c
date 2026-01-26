#include "supernova.h"

void reboot() {
    outb(0x64, 0xFE);
}

int reboot_delayed(int interval) {
    kprint("System rebooting...\n", -1, 0x0E);
    msleep(interval);
    outb(0x64, 0xFE);
}
