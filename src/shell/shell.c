#include "supernova.h"
#include "shell.h"

extern int cursor_x;
extern int cursor_y;
extern int input_start_x;

void get_command(char* out_buf) {
    int i = 0;
    // We access input_start_x and cursor_y which should be in supernova.h
    for (int x = input_start_x; x < 80; x++) {
        char c = get_char_at(x, cursor_y);
        out_buf[i++] = c;
    }

    i--;
    while (i >= 0 && out_buf[i] == ' ') {
        out_buf[i] = '\0';
        i--;
    }
}

void exec_command(char* command) {
    if (strcmp(command, "help") == 0) {
    	kprint("Fallback shell v0.1 (Hardcoded):\n", -1, 0x0E);
    	kprint("help    - Show this menu\n", -1, 0x07);
    	kprint("clear   - Clear the screen\n", -1, 0x07);
    	kprint("hi      - Welcome message\n", -1, 0x07);
    	kprint("fetch   - System info\n", -1, 0x07); // Added fetch!
    	kprint("version - Display versions\n", -1, 0x07);
    // putchar('\n', 0x07); // No need cuz the last kprint has a \n
    }
    else if (strcmp(command, "clear") == 0) {
        clear_screen();
    } 
    else if (strcmp(command, "hi") == 0) {
        kprint("Hello from the kernel!", -1, 0x0E);
        putchar('\n', 0x07);
    } 
    else if (strcmp(command, "fetch") == 0) {
    	kprint("      * ", -1, 0x0B); kprint("OS: Supernova\n", -1, 0x0F);
    	kprint("     / \\     ", -1, 0x0B); kprint("Kernel: 0.1-alpha\n", -1, 0x0F);
    	kprint("    < @ >    ", -1, 0x0B); kprint("Shell: Fallback (C-Hardcoded)\n", -1, 0x0F);
    	kprint("     \\ /     ", -1, 0x0B); kprint("Arch: x86_64\n", -1, 0x0F);
    	kprint("      v      ", -1, 0x0B); kprint("Status: Stable\n", -1, 0x0A);
}
    else if (strcmp(command, "version") == 0) {
        kprint("Supernova v0.1 \n Shell v0.1", -1, 0x0E);
        putchar('\n', 0x07);
    } 
    else if (command[0] == '\0') {
        // Just enter pressed
    }
    else {
        kprint("Unknown command: ", -1, 0x0C);
        kprint(command, -1, 0x0C);
        putchar('\n', 0x07);
    }
    print_prompt();
}

// The "Universal" bridge function
void shell_handle_enter() {
    char cmd_buf[81];
    get_command(cmd_buf);
    putchar('\n', 0x07);
    exec_command(cmd_buf);
}

void shell_exec() {
    print_prompt();
}
