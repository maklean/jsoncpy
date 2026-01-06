#include "include/debug.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdbool.h>

static void print_depth(int depth);

void debug_json_file(json_file *jf) {
    if(!jf) {
        fprintf(stderr, "Invalid json_file struct given.\n");
        return;
    }

    printf("=== json_file ===:\nName: %s\nLength: %ld\nContent: %s\n", jf->file_name, jf->length, jf->content);
}

void debug_scan_result(scan_result *sr) {
    if(!sr) {
        fprintf(stderr, "Invalid scan_result struct given.\n");
        return;
    }
    
    printf("=== scan_result ===:\nToken Count: %ld\nTokens: [", sr->token_count);

    for(size_t i = 0; i < sr->token_count; i++) {
        printf("\n\t{ TYPE: %s, VALUE: '%s' }", token_type_to_str(sr->tokens[i].type), sr->tokens[i].value);
        if(i != sr->token_count-1) printf(",");
    }
    printf("\n]\n");
}

void debug_node_tree(node *n, int depth) {
    if (!n) {
        fprintf(stderr, "Invalid node pointer given.\n");
        return;
    }

    // indent
    print_depth(depth);

    switch (n->type) {
        case NODE_STRING:
            printf("STRING: \"%s\"\n", (char *)n->value);
            break;
        case NODE_NUMBER_INT:
            printf("NUMBER (INT): %d\n", *(int *)n->value);
            break;
        case NODE_NUMBER_FLOAT:
            printf("NUMBER (FLOAT): %.17g\n", *(double *)n->value);
            break;
        case NODE_BOOLEAN:
            printf("BOOLEAN: %s\n", *(bool *)n->value ? "true" : "false");
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
        case NODE_OBJECT: {
            collection *coll = (collection *)n->value;

            printf("OBJECT (Length: %ld) {\n", coll->length);

            kv_pair *pairs = (kv_pair *)coll->collection;

            for(size_t i = 0; i < coll->length; i++) {
                for (int j = 0; j < depth + 1; j++) printf("  ");
                printf("KEY: %s\n", pairs[i].key);

                debug_node_tree((node *)pairs[i].value, depth + 2);
            }

            print_depth(depth);
            printf("}\n");
            break;
        }
        case NODE_ARRAY: {
            collection *coll = (collection *)n->value;

            printf("ARRAY (Length: %ld) [\n", coll->length);

            node *arr = (node *)coll->collection;

            for(size_t i = 0; i < coll->length; i++) {
                debug_node_tree(&arr[i], depth+2);
            }

            print_depth(depth);
            printf("]\n");

            break;
        }
        default:
            printf("UNKNOWN NODE\n");
    }
}

void print_depth(int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
}