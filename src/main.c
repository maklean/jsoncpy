#include "include/scanner.h"
#include "include/parser.h"
#include "include/debug.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Insufficent Arguments.\n");
        printf("Usage: ./executable_name <path-to-json>");

        exit(EXIT_FAILURE);
    }

    scan_result *sr = scan(argv[1]);
    if(!sr) {
        exit(EXIT_FAILURE);
    }

    node *root = parse(sr);
    if(!root) {
        free_scan_result(sr);
        exit(EXIT_FAILURE);
    }

    debug_node_tree(root, 0);

    free_scan_result(sr);
    free_node_ast(root);
    
    return 0;
}