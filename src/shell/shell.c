#include "supernova.h"
#include "shell.h"

extern int cursor_x;
extern int cursor_y;
extern int input_start_x;

/* --- The Command Structure --- */
typedef struct {
    char* name;
    char* description;
    void (*handler)(char* args); // Updated to accept a string of arguments
} command_t;

/* --- Forward Declarations --- */
void cmd_help(char* args);
void cmd_clear(char* args);
void cmd_hi(char* args);
void cmd_fetch(char* args);
void cmd_version(char* args);
void cmd_reboot(char* args);
void cmd_read(char* args);
void cmd_write(char* args);

/* --- The Modular Map --- */
command_t shell_commands[] = {
    {"help",    "Show this help menu",      cmd_help},
    {"clear",   "Clear the screen",         cmd_clear},
    {"hi",      "Print welcome message",    cmd_hi},
    {"fetch",   "Show system information",  cmd_fetch},
    {"version", "Show kernel version",      cmd_version},
    {"reboot",  "Restart the machine",      cmd_reboot},
    {"read",    "read <sector>",            cmd_read},
    {"write",   "write <sector> <message>", cmd_write}
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

    // Split command and arguments
    char cmd_name[32];
    char* args = "";
    int i = 0;
    
    // Copy first word into cmd_name
    while (input[i] != ' ' && input[i] != '\0' && i < 31) {
        cmd_name[i] = input[i];
        i++;
    }
    cmd_name[i] = '\0';

    // If there's a space, everything after it is 'args'
    if (input[i] == ' ') {
        args = &input[i + 1];
    }

    for (int j = 0; j < num_commands; j++) {
        if (strcmp(cmd_name, shell_commands[j].name) == 0) {
            shell_commands[j].handler(args);
            print_prompt();
            return;
        }
    }

    kprint("Unknown command: ", -1, 0x0C);
    kprint(cmd_name, -1, 0x0C);
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

void cmd_help(char* args) {
    kprint("Supernova Shell - Commands:\n", -1, 0x0E);
    for (int i = 0; i < num_commands; i++) {
        kprint("  ", -1, 0x07);
        kprint(shell_commands[i].name, -1, 0x0B);
        kprint(" - ", -1, 0x07);
        kprint(shell_commands[i].description, -1, 0x07);
        putchar('\n', 0x07);
    }
}

void cmd_clear(char* args) { clear_screen(); }
void cmd_hi(char* args) { kprint("Hello from the Supernova Kernel!\n", -1, 0x0E); }

void cmd_fetch(char* args) {
    char brand[49];
    get_cpu_brand(brand);

    kprint("      * ", -1, 0x0B); kprint("OS: Supernova\n", -1, 0x0F);
    kprint("     / \\     ", -1, 0x0B); kprint("Kernel: 0.1-alpha\n", -1, 0x0F);
    kprint("    < @ >    ", -1, 0x0B); kprint("CPU: ", -1, 0x0F); kprint(brand, -1, 0x0F); putchar('\n', 0x0F);
    kprint("     \\ /     ", -1, 0x0B); kprint("Arch: x86_64\n", -1, 0x0F);
    kprint("      v      ", -1, 0x0B); kprint("Status: Stable\n", -1, 0x0A);
}

void cmd_version(char* args) { kprint("Supernova v0.1.0-alpha\n", -1, 0x07); }
void cmd_reboot(char* args) { reboot(); }

void cmd_read(char* args) {
    if (*args == '\0') {
        kprint("Usage: read <sector>\n", -1, 0x0C);
        return;
    }

    uint32_t sector = str_to_int(args);
    uint16_t buffer[256]; 
    ata_read_sector(sector, buffer);

    char* data = (char*)buffer;
    for(int i = 0; i < 512; i++) {
        if(data[i] >= 32 && data[i] <= 126) putchar(data[i], 0x0A);
        else if(data[i] == '\n') putchar('\n', 0x07);
        else putchar('.', 0x08); // Gray dot for empty bytes
    }
    putchar('\n', 0x07);
}

void cmd_write(char* args) {
    if (*args == '\0') {
        kprint("Usage: write <sector> <message>\n", -1, 0x0C);
        return;
    }

    uint32_t sector = str_to_int(args);

    // Skip the sector number to find the message
    char* msg = args;
    while (*msg != ' ' && *msg != '\0') msg++;
    if (*msg == ' ') msg++; // Point to the text after the space

    uint16_t buffer[256];
    for(int i = 0; i < 512; i++) ((char*)buffer)[i] = ' '; // Wipe buffer
    
    for(int i = 0; msg[i] != '\0' && i < 511; i++) {
        ((char*)buffer)[i] = msg[i];
    }

    ata_write_sector(sector, buffer);
    kprint("Wrote to sector successfully.\n", -1, 0x0A);
}
