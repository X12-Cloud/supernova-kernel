#ifndef SHELL_H
#define SHELL_H

#define MAX_HISTORY 10

extern int history_browse_index;
extern int history_count;
extern char cmd_history[MAX_HISTORY][128];
extern char input_buffer[256];

void print_prompt();
void get_command(char* out_buf);
void exec_command(char* command);
void shell_handle_enter();
void shell_exec();
void shell_replace_line(char* new_command);

#endif
