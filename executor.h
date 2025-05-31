#ifndef EXECUTOR_H
#define EXECUTOR_H

// Executes the command with args
void execute_command(char **args);
int handle_builtin(char** args);
#endif

