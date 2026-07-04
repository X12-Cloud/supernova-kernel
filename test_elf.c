void _start() {
    volatile unsigned char* vga = (volatile unsigned char*)0xB8000;

    for (int i = 0; i < 4000; i += 2) {
        vga[i]     = ' ';    // Clear text with a space character
        vga[i + 1] = 0x1F;   // 0x1 = Blue background, 0xF = White text
    }

    const char* line1 = "  ____  _   _ _  __ ";
    const char* line2 = " / ___|| \\ | | |/ / ";
    const char* line3 = " \\___ \\|  \\| | ' /  ";
    const char* line4 = "  ___) | |\\  | . \\  ";
    const char* line5 = " |____/|_| \\_|_|\\_\\ ";
    const char* sub   = " [ SUPERNOVA KERNEL RUNNING USER SPACE ]";

    int start_row = 9;
    int start_col = 28; // Center horizontally

    const char* Lines[5] = {line1, line2, line3, line4, line5};

    for (int r = 0; r < 5; r++) {
        int vga_offset = ((start_row + r) * 80 + start_col) * 2;
        for (int c = 0; Lines[r][c] != '\0'; c++) {
            vga[vga_offset + (c * 2)]     = Lines[r][c];
            vga[vga_offset + (c * 2) + 1] = 0x1E; // Blue background, Yellow text!
        }
    }

    // Print the subtitle row underneath
    int sub_offset = ((start_row + 6) * 80 + 19) * 2;
    for (int c = 0; sub[c] != '\0'; c++) {
        vga[sub_offset + (c * 2)]     = sub[c];
        vga[sub_offset + (c * 2) + 1] = 0x1A; // Blue background, Light green subtitle
    }

    // Infinite loop
    while(1) {
        __asm__ volatile("nop");
    }
}
