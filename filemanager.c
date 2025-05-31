#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#define MAX_FILES 1024
#define NAME_MAX 255

typedef struct {
    char name[NAME_MAX];
    bool is_dir;
    off_t size;
    time_t last_modified;
} FileMeta;

static FileMeta* get_file_metadata(const char* path) {
    struct stat st;
    if (stat(path, &st)) {
        perror("stat");
        return NULL;
    }

    FileMeta* meta = malloc(sizeof(FileMeta));
    if (!meta) {
        perror("malloc");
        return NULL;
    }

    const char* basename = strrchr(path, '/');
    strncpy(meta->name, basename ? basename + 1 : path, NAME_MAX);
    meta->is_dir = S_ISDIR(st.st_mode);
    meta->size = st.st_size;
    meta->last_modified = st.st_mtime;
    return meta;
}

static bool is_text_file(const char* path) {
    const char* ext = strrchr(path, '.');
    if (ext) {
        const char* text_exts[] = {".c", ".h", ".txt", ".md", ".cpp", ".hpp", ".py", NULL};
        for (int i = 0; text_exts[i]; i++) {
            if (strcasecmp(ext, text_exts[i]) == 0) return true;
        }
    }

    FILE* f = fopen(path, "rb");
    if (!f) return false;
    
    unsigned char buf[1024];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    
    for (size_t i = 0; i < n; i++) {
        if (buf[i] == 0 || (buf[i] < 32 && buf[i] != '\n' && buf[i] != '\t' && buf[i] != '\r')) {
            return false;
        }
    }
    return true;
}

void launch_file_manager() {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }

    while (1) {
        system("clear");
        printf("\033[1;34m📁 Current Directory: %s\033[0m\n\n", cwd);

        DIR *dir = opendir(cwd);
        if (!dir) {
            perror("opendir");
            return;
        }

        FileMeta* files[MAX_FILES];
        int count = 0;
        struct dirent *entry;

        while ((entry = readdir(dir)) && count < MAX_FILES) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char fullpath[PATH_MAX];
            int w = snprintf(fullpath, sizeof(fullpath), "%s/%s", cwd, entry->d_name);
            if (w < 0 || w >= sizeof(fullpath)) {
                fprintf(stderr, "Path too long\n");
                continue;
            }

            files[count] = get_file_metadata(fullpath);
            if (files[count]) {
                printf(" \033[1;33m[%d]\033[0m %c %-30s \033[90m%6ld KB\033[0m\n",
                       count,
                       files[count]->is_dir ? 'D' : 'F',
                       files[count]->name,
                       files[count]->size / 1024);
                count++;
            }
        }
        closedir(dir);

        printf("\nEnter number to select, '..' to go back, 'q' to quit:\n> ");
        
        char input[100];
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "q") == 0) {
            for (int i = 0; i < count; i++) free(files[i]);
            break;
        }
        
        if (strcmp(input, "..") == 0) {
            chdir("..");
            getcwd(cwd, sizeof(cwd));
            continue;
        }

        int choice = atoi(input);
        if (choice < 0 || choice >= count) continue;

        char path[PATH_MAX];
        int w = snprintf(path, sizeof(path), "%s/%s", cwd, files[choice]->name);
        if (w < 0 || w >= sizeof(path)) {
            fprintf(stderr, "Path too long\n");
            continue;
        }

        while (1) {
            system("clear");
            printf("Selected: \033[1;32m%s\033[0m\n\n", files[choice]->name);
            printf(" [o] Open %s\n", files[choice]->is_dir ? "(Enter Dir)" : "(View File)");
            printf(" [d] Delete\n");
            printf(" [r] Rename\n");
            printf(" [b] Back\n> ");

            if (!fgets(input, sizeof(input), stdin)) break;
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "b") == 0) break;

            if (strcmp(input, "o") == 0) {
                if (files[choice]->is_dir) {
                    chdir(path);
                    getcwd(cwd, sizeof(cwd));
                    break;
                } else {
                    char cmd[PATH_MAX + 16];
                    w = snprintf(cmd, sizeof(cmd), "less \"%s\"", path);
                    if (w < 0 || w >= sizeof(cmd)) {
                        fprintf(stderr, "Command too long\n");
                        continue;
                    }

                    system("tput smcup");  // Save screen
                    int ret = system(cmd);
                    system("tput rmcup");  // Restore screen

                    if (ret == -1) {
                        perror("Failed to open file");
                    }
                    break;
                }
            } 
            else if (strcmp(input, "d") == 0) {
                int success = files[choice]->is_dir ? rmdir(path) : remove(path);
                if (success == 0) {
                    printf("\033[32mDeleted successfully.\033[0m\n");
                } else {
                    perror("\033[31mdelete\033[0m");
                }
                printf("Press Enter to continue...");
                fgets(input, sizeof(input), stdin);
                break;
            } 
            else if (strcmp(input, "r") == 0) {
                printf("New name: ");
                char newname[NAME_MAX];
                if (!fgets(newname, sizeof(newname), stdin)) break;
                newname[strcspn(newname, "\n")] = 0;

                char newpath[PATH_MAX];
                w = snprintf(newpath, sizeof(newpath), "%s/%s", cwd, newname);
                if (w < 0 || w >= sizeof(newpath)) {
                    fprintf(stderr, "New path too long\n");
                    continue;
                }

                if (rename(path, newpath) == 0) {
                    printf("\033[32mRenamed successfully.\033[0m\n");
                } else {
                    perror("\033[31mrename\033[0m");
                }
                printf("Press Enter to continue...");
                fgets(input, sizeof(input), stdin);
                break;
            }
        }

        for (int i = 0; i < count; i++) free(files[i]);
        getcwd(cwd, sizeof(cwd));
    }
}
