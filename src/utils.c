#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>

static void free_object_collection(collection *coll);
static void free_array_collection(collection *coll);

void free_scan_result(scan_result *sr) {
    if(!sr) {
        fprintf(stderr, "Invalid scan_result struct object given.\n");
        return;
    }

    //if(sr->json_file) free_json_file(sr->json_file);
    if(sr->tokens) free(sr->tokens);

    free(sr);
}

void free_json_file(json_file *jf) {
    if(!jf) {
        fprintf(stderr, "Invalid json_file struct object given.\n");
        return;
    }

    if(jf->content) free(jf->content);
    
    free(jf);
}

static void free_object_collection(collection *coll) {
    // free the value pointer from every pair
    kv_pair *pairs = (kv_pair *)coll->collection;

    for(size_t i = 0; i < coll->length; i++) {
        free_node_ast(pairs[i].value);
    }

    free(pairs);
}

static void free_array_collection(collection *coll) {
    // free the value pointer from every array element.
    node *arr_elements = (node *)coll->collection;

    for(size_t i = 0; i < coll->length; i++) {
        if(arr_elements[i].type == NODE_OBJECT) {
            free_object_collection(arr_elements[i].value);
        } else if(arr_elements[i].type == NODE_ARRAY) {
            free_array_collection(arr_elements[i].value);
        } 

        if(arr_elements[i].value) free(arr_elements[i].value);
    }
    
    free(arr_elements);
}

void free_node_ast(node *n) {
    if (!n) {
        fprintf(stderr, "Invalid node pointer given.\n");
        return;
    }

    if(n->type == NODE_OBJECT) {
        collection *coll = (collection *)n->value;
        free_object_collection(coll);
    } else if(n->type == NODE_ARRAY) {
        collection *coll = (collection *)n->value;
        free_array_collection(coll);
    }

    if(n->value) {
        free(n->value);
    }
    free(n);
}

const char *token_type_to_str(token_type t) {
    switch(t) {
        case BEGIN_ARRAY:
            return "BEGIN_ARRAY";
        case END_ARRAY:
            return "END_ARRAY";
        case BEGIN_OBJECT:
            return "BEGIN_OBJECT";
        case END_OBJECT:
            return "END_OBJECT";
        case NAME_SEPARATOR:
            return "NAME_SEPARATOR";
        case VALUE_SEPARATOR:
            return "VALUE_SEPARATOR";
        case LT_FALSE:
            return "LT_FALSE";
        case LT_TRUE:
            return "LT_TRUE";
        case LT_NULL:
            return "LT_NULL";
        case STRING:
            return "STRING";
        case NUMBER:
            return "NUMBER";
        default:
            return "UNKNOWN";
    }
}