#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

bool dir_exist(char *path) {
    struct stat sb{};

    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    } else {
        return false;
    }
}

bool file_exist(char *path) {
    struct stat sb{};

    if (stat(path, &sb) == 0 && S_ISREG(sb.st_mode)) {
        return true;
    } else {
        return false;
    }
}

int open_file(const char *file_path, const char *modes) {
    FILE* fp = fopen(file_path, modes);
    return fileno(fp);
}

char **get_file_lines(const char *path, int *size) {
    *size = 0;

    FILE* fp = fopen(path, "r");
    char* line;
    size_t line_size = 0;

    while ((getline(&line, &line_size, fp)) != -1) {
        ++(*size);
    }

    if (*size == 0)
        return NULL;

    fseek(fp, 0, SEEK_SET);

    char** lines = (char**)malloc(*size * sizeof(char*));


    for (int i =0 ; i < *size; ++i) {
        getline(&line, &line_size, fp);
        lines[i] = (char*)calloc(line_size, 1);
        strcpy(lines[i], line);
    }

    free(line);
    fclose(fp);
    return lines;
}
