#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

#define HISTORY_FILE ".pepshell_history"

void add_to_history(const char *line) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        perror("Could not open history file");
        return;
    }
    fprintf(file, "%s\n", line);
    fclose(file);
}

void show_history() {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (file == NULL) {
        printf("No history found.\n");
        return;
    }
    char line[1024];
    int count = 1;
    while (fgets(line, sizeof(line), file)) {
        printf("%d  %s", count++, line);
    }
    fclose(file);
}

