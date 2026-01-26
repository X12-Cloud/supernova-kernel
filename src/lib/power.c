#include "supernova.h"

void reboot() { outb(0x64, 0xFE); }
