#include <stdio.h>
#include <string.h>
#include "parser.h"

int parse_input(char *input, char **args) {
    int i = 0;
    // Remove trailing newline if any
    input[strcspn(input, "\n")] = 0;

    // Tokenize by space
    char *token = strtok(input, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // NULL-terminate the args array
    return i;
}

int background(char** args){
	int i=0;
	while(args[i]){
		i++;
	}
	while(i>0 && strcmp(args[i-1],"&")==0){
		args[i-1]=NULL;
		return 1;
	}
	return 0;
}
