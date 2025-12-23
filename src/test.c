#include <stdio.h>
#include <stdlib.h>
#include "include/scanner.h"
#include "include/utils.h"

int main() {
    char *file_path = "../test.json";

    scan_result *sr = scan(file_path);
    if(!sr) {
        return 0;
    }

    free_scan_result(sr);

    return 0;
}