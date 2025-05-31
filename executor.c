#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"
#include <string.h>
#include "filemanager.h"

void execute_command(char **args) {
    int background=0;
    int i = 0;
    while (args[i] != NULL) i++;
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        background = 1;
        args[i - 1] = NULL; // Remove '&'
    }
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Child process
       if (strcmp(args[0], "fileman") == 0) {
           launch_file_manager();
            _exit(0);
        }
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
        }
        _exit(1); // Exit child process if execvp fails
    } else {
        // Parent
        if (background) {
            printf("[Background PID %d]\n", pid);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
int handle_builtin(char** args){
	if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "mysh: expected argument to \"cd\"\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("mysh");
            }
        }
        return 1;  // handled built-in command
    }
	return 0;
}
