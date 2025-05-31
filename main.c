#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "executor.h"
#include "history.h"

#define MAX_INPUT 1024

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf("pepshell> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break; // Exit on Ctrl+D
        }
	add_to_history(input);
        // Parse input
        int arg_count = parse_input(input, args);

        if (arg_count == 0) continue; // Empty input, reprompt

	if (strcmp(args[0], "exit") == 0) break;

        if (strcmp(args[0], "history") == 0) {
            show_history();
            continue;
        }

	if (handle_builtin(args)) {
	    continue;
	}
	execute_command(args);
    }

    printf("Goodbye!\n");
    return 0;
}

