#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64

// Parses input line into args array; returns number of args
int parse_input(char *input, char **args);
int background(char** args);
#endif

