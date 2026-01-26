#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include "supernova.h"

// Command structure
typedef struct {
    char* name;         // The string user types (e.g. "help")
    char* description;  // What shows up in the help menu
    void (*handler)();  // A "Function Pointer" – the function to run
} command_t;

// Global functions
void print_prompt();
void get_command(char* out_buf);
void exec_command(char* command);
void shell_handle_enter();
void shell_exec();

#endif
