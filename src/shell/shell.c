#include "supernova.h"

extern int cursor_x;
extern int cursor_y;
extern int input_start_x;

int history_count = 0;
int history_browse_index = -1;
char cmd_history[MAX_HISTORY][128];
char input_buffer[256];

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
void cmd_hex(char* args);
void cmd_ls(char* args);
void cmd_cat(char* args);
void cmd_echo(char* args);
void cmd_touch(char* args);
void cmd_rm(char* args);
// void cmd_history(char* args);

/* --- The Modular Map --- */
command_t shell_commands[] = {
    {"help",    "Show this help menu",      cmd_help},
    {"clear",   "Clear the screen",         cmd_clear},
    {"hi",      "Print welcome message",    cmd_hi},
    {"fetch",   "Show system information",  cmd_fetch},
    {"version", "Show kernel version",      cmd_version},
    {"reboot",  "Restart the machine",      cmd_reboot},
    {"read",    "read <sector>",            cmd_read},
    {"write",   "write <sector> <message>", cmd_write},
    {"hex",     "hex <sector> - Hexdump the sector", cmd_hex},
    {"ls",      "ls - List the file contents", cmd_ls},
    {"cat",     "cat - List the contents of a file", cmd_cat},
    {"echo",    "Print argument/message",           cmd_echo},
    {"touch",   "Make a new file",          cmd_touch},
    {"rm",      "rm <filename> - Delete file",    cmd_rm},
    // {"history", "Show command history",     cmd_history},
};

const int num_commands = sizeof(shell_commands) / sizeof(command_t);

/* --- Shell Engine --- */

void print_prompt() {
    kprint("Supernova> ", -1, 0x0B); // Print in Cyan at current position
    input_start_x = cursor_x;       // Lock the "Safe Zone" to the current cursor spot
}

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
    get_command(input_buffer);

    if (strlen(input_buffer) > 0 && history_count < MAX_HISTORY) {
        strcpy(cmd_history[history_count], input_buffer);
        history_count++;
    }
    
    history_browse_index = history_count;

    putchar('\n', 0x07);
    exec_command(input_buffer);
}

void shell_replace_line(char* new_command) {
    while (cursor_x > input_start_x) {
        putchar('\b', 0x07);
    }

    kprint(new_command, -1, 0x0F);
    
    strcpy(input_buffer, new_command); 
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

void cmd_echo(char* args) {
    char* redir = 0;
    /* int file_found = 0;
    int index = fat32_find_file(args);
    if (index != -1) { file_found = 1; } */
    // Look for '>'
    for (int i = 0; args[i] != '\0'; i++) {
        if (args[i] == '>') {
            args[i] = '\0'; // Terminate the message string here
            redir = &args[i + 1];
            break;
        }
    }

    if (redir) {
        while (*redir == ' ') redir++;
	int index = fat32_find_file(redir);
        
        if (index == -1) { 
            fat32_create_file(redir); 
        }
        // Write data (We need a fat32_write_file function for this!)
        kprint("Redirecting to file...\n", -1, 0x0B);
	fat32_write_file(redir, args);
    } else {
        kprint(args, -1, 0x0E);
        putchar('\n', 0x07);
    }
}

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

void cmd_hex(char* args) {
    uint32_t sector = 0;
    // If user typed "hex 5", read sector 5. Otherwise, read 0.
    if (args && *args != '\0') {
        sector = str_to_int(args);
    }

    uint16_t buffer[256]; 
    ata_read_sector(sector, buffer);

    uint8_t* ptr = (uint8_t*)buffer;
    
    for (int i = 0; i < 512; i += 16) {
        // 1. Offset (e.g. 0000, 0010)
        print_hex_byte((i >> 8) & 0xFF);
        print_hex_byte(i & 0xFF);
        kprint("  ", -1, 0x07);

        // 2. Hex data
        for (int j = 0; j < 16; j++) {
            print_hex_byte(ptr[i + j]);
            putchar(' ', 0x07);
            if (j == 7) kprint(" ", -1, 0x07); // Middle gap
        }

        kprint(" |", -1, 0x08);

        // 3. ASCII sidebar
        for (int j = 0; j < 16; j++) {
            char c = ptr[i + j];
            if (c >= 32 && c <= 126) {
                putchar(c, 0x0F); // Readable
            } else {
                putchar('.', 0x08); // Non-printable
            }
        }
        kprint("|\n", -1, 0x08);
    }
}

void cmd_ls(char* args) {
    uint8_t buf[512];

    if (*args != '\0') {
        // We are searching for a specific file
        int index = fat32_find_file(args);
        if (index != -1) {
            kprint("File Found: ", -1, 0x0A);
            kprint(args, -1, 0x0F);
            putchar('\n', 0x07);
        } else {
            kprint("File Not Found: ", -1, 0x0C);
            kprint(args, -1, 0x0F);
            putchar('\n', 0x07);
        }
        return;
    }
    
    // Use the variable we calculated in fat32_init
    extern uint32_t root_dir_sector; 
    ata_read_sector(root_dir_sector, (uint16_t*)buf);

    kprint("Directory Listing:\n", -1, 0x0E);

    for (int i = 0; i < 512; i += 32) {
        // If the first byte is 0, there are no more files
        if (buf[i] == 0x00) break;
        
        // If the first byte is 0xE5, the file was deleted
        if (buf[i] == 0xE5) continue;

        // Skip "Long File Name" entries (Attribute byte at offset 11 is 0x0F)
        if (buf[i + 11] == 0x0F) continue;

        // Print the 8-char filename
        for (int j = 0; j < 8; j++) {
            if (buf[i + j] != ' ') putchar(buf[i + j], 0x0F);
        }

        // Print a dot and the 3-char extension
        putchar('.', 0x07);
        for (int j = 8; j < 11; j++) {
            if (buf[i + j] != ' ') putchar(buf[i + j], 0x0F);
        }
        
        putchar('\n', 0x07);
    }
}

void cmd_cat(char* args) {
    if (*args == '\0') {
        kprint("Usage: cat <filename.ext>\n", -1, 0x0C);
        return;
    }

    char target[11];
    format_to_83(args, target); // Convert "hello.txt" to "HELLO   TXT"

    uint8_t buf[512];
    extern uint32_t root_dir_sector;
    ata_read_sector(root_dir_sector, (uint16_t*)buf);

    for (int i = 0; i < 512; i += 32) {
        if (buf[i] == 0x00) break;
        if (buf[i] == 0xE5) continue;

        if (memcmp(&buf[i], target, 11) == 0) {
            uint16_t cluster = *(uint16_t*)&buf[i + 26];
            uint32_t sector = root_dir_sector + (cluster - 2);

            uint16_t file_data[256];
            ata_read_sector(sector, file_data);
            
            kprint((char*)file_data, -1, 0x0F);
            putchar('\n', 0x07);
            return;
        }
    }
    kprint("File not found.\n", -1, 0x0C);
}

void cmd_touch(char* args) {
    fat32_create_file(args);
}

void cmd_rm(char* args) { fat32_delete_file(args); }

/* void cmd_history(char* args) {
    kprint("Recent Commands:\n", -1, 0x0E);
    for (int i = 0; i < history_count; i++) {
        kprint("  ", -1, 0x07);
        kprint(cmd_history[i], -1, 0x0F);
        putchar('\n', 0x07);
    }
} */
