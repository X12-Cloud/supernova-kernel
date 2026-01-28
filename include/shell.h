#ifndef SHELL_H
#define SHELL_H

#include "supernova.h"

void print_prompt();
void get_command(char* out_buf);
void exec_command(char* command);
void shell_handle_enter();
void shell_exec();

#endif
