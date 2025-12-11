#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/loader.h"

char *load_file(char *path) {
    // only read .json files.
    if(strcmp(strrchr(path, '.'), ".json") != 0) {
        fprintf(stderr, "%s is not a JSON file.\n", path);
        return NULL;
    }

    FILE *f = fopen(path, "rb");
    if(!f) {
        perror("failed to open file at given path");
        return NULL;
    }

    // get length of content by moving to the end of the file.
    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    rewind(f);

    char *content = malloc(length+1);
    if(!content) {
        perror("failed to allocate for content string");
        return NULL;
    }

    fread(content, 1, length, f);
    content[length] = '\0';

    fclose(f);

    return content;
}