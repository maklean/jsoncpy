#include <stdio.h>
#include <stdlib.h>
#include "include/loader.h"

int main() {
    char *file_path = "../test.json";

    char *content = load_file(file_path);

    printf("%s\n", content);

    free(content);

    return 0;
}