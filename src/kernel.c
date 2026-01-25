void clear_screen() {
    char* vidmem = (char*)0xB8000;
    // VGA text mode is 80 columns * 25 rows
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vidmem[i] = ' ';     // Character
        vidmem[i+1] = 0x07;  // Light grey on black attribute
    }
}

void kprint(const char* str, int row) {
    char* vidmem = (char*)0xB8000 + (row * 80 * 2);
    for (int i = 0; str[i] != '\0'; i++) {
        vidmem[i*2] = str[i];
        vidmem[i*2 + 1] = 0x0A; // Light Green
    }
}

void kernel_main() {
    clear_screen();
    kprint("Supernova Kernel v0.1 Loaded", 0);
    kprint("Status: Running in 32-bit Protected Mode", 1);
    kprint("Arch: x86_2026", 2);

    while(1);
}
