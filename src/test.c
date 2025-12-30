#include <stdio.h>
#include <stdlib.h>
#include "include/scanner.h"
#include "include/utils.h"
#include "include/debug.h"
#include "include/parser.h"

int main() {
    char *file_path = "../test.json";

    scan_result *sr = scan(file_path);
    if(!sr) {
        fprintf(stderr, "Failed to scan.\n");
        return 0;
    }

    debug_scan_result(sr);

    node *n = parse(sr);
    if(!n) {
        fprintf(stderr, "Failed to parse.\n");
        return 0;
    }

    debug_node_tree(n, 0);

    free_scan_result(sr);

    return 0;
}