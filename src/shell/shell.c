#include "supernova.h"
#include "shell.h"

extern int cursor_x;
extern int cursor_y;
extern int input_start_x;

/* --- The Command Structure --- */
typedef struct {
    char* name;
    char* description;
    void (*handler)();
} command_t;

/* --- Forward Declarations --- */
void cmd_help();
void cmd_clear();
void cmd_hi();
void cmd_fetch();
void cmd_version();
void cmd_reboot();

/* --- The Modular Map --- */
command_t shell_commands[] = {
    {"help",    "Show this help menu",      cmd_help},
    {"clear",   "Clear the screen",         cmd_clear},
    {"hi",      "Print welcome message",    cmd_hi},
    {"fetch",   "Show system information",  cmd_fetch},
    {"version", "Show kernel version",      cmd_version},
    {"reboot",  "Restart the machine",      cmd_reboot}
};

const int num_commands = sizeof(shell_commands) / sizeof(command_t);

/* --- Shell Engine --- */

void get_command(char* out_buf) {
    int i = 0;
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

void exec_command(char* input) {
    if (input[0] == '\0') {
        print_prompt();
        return;
    }

    for (int i = 0; i < num_commands; i++) {
        if (strcmp(input, shell_commands[i].name) == 0) {
            shell_commands[i].handler();
            print_prompt();
            return;
        }
    }

    kprint("Unknown command: ", -1, 0x0C);
    kprint(input, -1, 0x0C);
    putchar('\n', 0x07);
    print_prompt();
}

void shell_handle_enter() {
    char cmd_buf[81];
    get_command(cmd_buf);
    putchar('\n', 0x07);
    exec_command(cmd_buf);
}

void shell_exec() {
    print_prompt();
}

/* --- Command Implementations --- */

void cmd_help() {
    kprint("Supernova Fallback Shell - Available Commands:\n", -1, 0x0E);
    for (int i = 0; i < num_commands; i++) {
        kprint("  ", -1, 0x07);
        kprint(shell_commands[i].name, -1, 0x0B); // Cyan
        kprint(" - ", -1, 0x07);
        kprint(shell_commands[i].description, -1, 0x07); // Gray/White
        putchar('\n', 0x07);
    }
}

void cmd_clear() {
    clear_screen();
}

void cmd_hi() {
    kprint("Hello from the Supernova Kernel!\n", -1, 0x0E);
}

void cmd_fetch() {
    kprint("      * ", -1, 0x0B); kprint("OS: Supernova\n", -1, 0x0F);
    kprint("     / \\     ", -1, 0x0B); kprint("Kernel: 0.1-alpha\n", -1, 0x0F);
    kprint("    < @ >    ", -1, 0x0B); kprint("Arch: x86_64\n", -1, 0x0F);
    kprint("     \\ /     ", -1, 0x0B); kprint("Console: VGA Text\n", -1, 0x0F);
    kprint("      v      ", -1, 0x0B); kprint("Status: Stable\n", -1, 0x0A);
}

void cmd_version() {
    kprint("Supernova Kernel Version 0.1.0-alpha (built 2026)\n", -1, 0x07);
}

void cmd_reboot() {
    reboot();
}
