#include "include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Parses the next token into node 'n', returns 0 if successful, otherwise -1.
static int parse_value(node *n);

// Parses the next token (i.e., BEGIN_OBJECT) into node 'n', returns 0 if successful, otherwise -1.
static int parse_object(node *n);

// Parses the next token (i.e., BEGIN_ARRAY) into node 'n', returns 0 if successful, otherwise -1.
static int parse_array(node *n);

static token *stream = NULL; // array of tokens
static size_t current; // current token index in 'stream'

node *parse(scan_result *sr) {
    if(!sr || !sr->tokens || sr->token_count == 0) {
        fprintf(stderr, "Cannot parse the given scan_result struct.\n");
        return NULL;
    }

    current = 0;
    stream = sr->tokens;

    node *n = malloc(sizeof(node));
    if(!n) {
        perror("Failed to allocate for new node");
        return NULL;
    }

    if(parse_value(n) != 0) {
        free(n);
        return NULL;
    }

    return n;
}

static int parse_value(node *n) {
    if(!n) {
        fprintf(stderr, "Invalid pointer to node struct given.\n");
        return -1;
    }

    token t = stream[current];

    switch(t.type) {
        case STRING:
            n->type = NODE_STRING;
            n->value = (char *)malloc(sizeof(t.value));

            if(!n->value) {
                perror("Failed to allocate memory for node value");
                return -1;
            }

            strcpy(n->value, t.value);
            return 0;
        case NUMBER: 
            int is_float = strchr(t.value, '.') != NULL;
            n->type = is_float ? NODE_NUMBER_FLOAT : NODE_NUMBER_INT;
            n->value = n->type == NODE_NUMBER_FLOAT ? malloc(sizeof(double)) : malloc(sizeof(int));

            if(!n->value) {
                perror("Failed to allocate memory for node value");
                return -1;
            }

            if(n->type == NODE_NUMBER_FLOAT) {
                *(double *)n->value = strtod(t.value, NULL);
            } else {
                *(int *)n->value = atoi(t.value);
            }
            
            return 0;
        case LT_TRUE:
        case LT_FALSE:
            n->type = NODE_BOOLEAN;
            n->value = malloc(sizeof(bool));

            if(!n->value) {
                perror("Failed to allocate memory for node value");
                return -1;
            }

            *(bool *)n->value = strcmp(t.value, "true") == 0 ? true : false;

            return 0;
        case LT_NULL:
            n->type = NODE_NULL;
            n->value = NULL;
            return 0;
        case BEGIN_OBJECT:
            return parse_object(n);
        case BEGIN_ARRAY:
            return parse_array(n);
    }

    fprintf(stderr, "Unable to parse value.\n");
    return -1;
}

static int parse_object(node *n) {
    if(!n) {
        fprintf(stderr, "Invalid pointer to node struct given.\n");
        return -1;
    }

    n->type = NODE_OBJECT;

    // array of kv_pairs
    kv_pair *pairs = NULL;
    kv_pair *tmp_pairs; // for realloc

    size_t next_index = 0;

    token t = stream[++current]; // move past '{'

    while(t.type != END_OBJECT) {
        if(t.type == STRING) {
            kv_pair pair;

            strcpy(pair.key, t.value);

            // name seperator should be the next token
            t = stream[++current];
            if(t.type != NAME_SEPARATOR) {
                fprintf(stderr, "Expected ':' at position %ld, but didn't find it.\n", current-1);
                return -1;
            }

            // parse next token into pair.value
            current++;

            pair.value = malloc(sizeof(node));
            if(parse_value(pair.value) != 0) {
                if(pairs) free(pairs);
                return -1;
            }

            // add kv_pair into the array
            tmp_pairs = realloc(pairs, sizeof(kv_pair)*(next_index+1));
            if(!tmp_pairs) {
                perror("Failed to reallocate kv_pair array");
                return -1;
            }
            pairs = tmp_pairs;

            pairs[next_index++] = pair;
        } else {
            fprintf(stderr, "Expected object key at token position: %ld.\n", current-1);
            if(pairs) free(pairs);
            return -1;
        }

        t = stream[++current];
        if(t.type == VALUE_SEPARATOR) {
            t = stream[++current];

            // there should be another key after a value seperator
            // this is more of a grammar check, this could be omitted and it would still get the complete AST.
            if(t.type == END_OBJECT) {
                fprintf(stderr, "Expected object key at token position: %ld.\n", current-1);
                if(pairs) free(pairs);
                return -1;
            }
        }
    }

    n->value = malloc(sizeof(collection));
    if(!n->value) {
        perror("Failed to allocate collection for object node value");
        if(pairs) free(pairs);
        return -1;
    }

    ((collection *)n->value)->collection = (void *)pairs;
    ((collection *)n->value)->length = next_index;

    return 0;
}

static int parse_array(node *n) {
    if(!n) {
        fprintf(stderr, "Invalid pointer to node struct given.\n");
        return -1;
    }

    n->type = NODE_ARRAY;

    node *arr_elements = NULL; // array of nodes
    node *tmp_arr_elements; // for realloc

    size_t i = 0; // to ensure correct value seperator and element position
    size_t next_index = 0;

    token t = stream[++current]; // move past '['

    while(t.type != END_ARRAY) {
        if(t.type == VALUE_SEPARATOR) {
            // value seperator should always be at an odd index.
            if(i % 2 != 1) {
                fprintf(stderr, "Unexpected value seperator at position %ld.\n", current-1);
                if(arr_elements) free(arr_elements);
                return -1;
            }

            // move on to the next token
            t = stream[++current];

            // there should be a new value
            if(t.type == END_ARRAY) {
                fprintf(stderr, "Expected value at position %ld, got end of array.\n", current-1);
                if(arr_elements) free(arr_elements);
                return -1;
            }

            i++;
        } else {
            // element should always be at an even index.
            if(i % 2 != 0) {
                fprintf(stderr, "Unexpected value at position %ld, expected value seperator.\n", current-1);
                if(arr_elements) free(arr_elements);
                return -1;
            }

            // parse value into arr_val
            node arr_val;
            if(parse_value(&arr_val) != 0) {
                if(arr_elements) free(arr_elements);
                return -1;
            }

            // add node to arr_elements
            tmp_arr_elements = realloc(arr_elements, sizeof(node)*(next_index+1));
            if(!tmp_arr_elements) {
                perror("Failed to reallocate for arr_elements");
                if(arr_elements) free(arr_elements);
                return -1;
            }

            arr_elements = tmp_arr_elements;
            arr_elements[next_index++] = arr_val;

            t = stream[++current];
            i++;
        }
    }

    n->value = malloc(sizeof(collection));
    ((collection *)n->value)->collection = (void *)arr_elements;
    ((collection *)n->value)->length = next_index;

    return 0;
}
