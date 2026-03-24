#include "include/scanner.h"
#include "include/parser.h"
#include "include/debug.h"
#include "include/utils.h"
#include "include/arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArenaBlock* block = NULL;

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Insufficent Arguments.\n");
        printf("Usage: ./jsoncpy <path-to-json>\n");

        exit(EXIT_FAILURE);
    }

    arena_init(&block, 0);
    if(block == NULL) {
        fprintf(stderr, "Failed to allocate arena block.\n");
        exit(EXIT_FAILURE);
    }

    scan_result *sr = scan(argv[1]);
    if(!sr) {
        arena_clean(&block);
        exit(EXIT_FAILURE);
    }

    node *root = parse(sr);
    if(!root) {
        arena_clean(&block);
        exit(EXIT_FAILURE);
    }

    debug_node_tree(root, 0);

    arena_clean(&block);
    
    return 0;
}