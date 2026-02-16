#include "include/scanner.h"
#include "include/parser.h"
#include "include/debug.h"
#include "include/utils.h"
#include "include/arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Arena *a;

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Insufficent Arguments.\n");
        printf("Usage: ./jsoncpy <path-to-json>\n");

        exit(EXIT_FAILURE);
    }

    // mother of god and all unholy programming practices.
    if((a = arena_init(MAX_ARENA_SIZE_MB * 1024 * 1024)) == NULL) {
        printf("Failed to initialize arena.\n");
        exit(EXIT_FAILURE);
    }

    scan_result *sr = scan(argv[1]);
    if(!sr) {
        arena_free(a);
        exit(EXIT_FAILURE);
    }

    node *root = parse(sr);
    if(!root) {
        arena_free(a);
        exit(EXIT_FAILURE);
    }

    debug_node_tree(root, 0);

    arena_free(a);
    
    return 0;
}