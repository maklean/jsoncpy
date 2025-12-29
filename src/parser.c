#include "include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static token *next_token(token *t_stream);

static int parse_value(node *n);
static int parse_object(node *n);
static int parse_array(node *n);

static size_t current;
static token *stream = NULL;

static void traverse_node(node *n, int depth) {
    if (!n) return;

    // indent
    for (int i = 0; i < depth; i++) printf("  ");

    collection *coll;

    switch (n->type) {
        case NODE_STRING:
            printf("STRING: %s\n", (char *)n->value);
            break;
        case NODE_NUMBER_INT:
            printf("NUMBER (INT): %d\n", *(int *)n->value);
            break;
        case NODE_NUMBER_FLOAT:
            printf("NUMBER (FLOAT): %f\n", *(double *)n->value);
            break;
        case NODE_BOOLEAN:
            printf("BOOLEAN: %s\n", *(int *)n->value ? "true" : "false");
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
        case NODE_OBJECT:
            coll = (collection *)n->value;

            printf("OBJECT (Length: %ld) {\n", coll->length);

            kv_pair *pairs = (kv_pair *)coll->collection;

            for(size_t i = 0; i < coll->length; i++) {
                for (int j = 0; j < depth + 1; j++) printf("  ");
                printf("KEY: %s\n", pairs[i].key);

                traverse_node((node *)pairs[i].value, depth + 2);
            }

            for (int i = 0; i < depth; i++) printf("  ");
            printf("}\n");
            break;
        case NODE_ARRAY:
            coll = (collection *)n->value;

            printf("ARRAY (Length: %ld) [\n", coll->length);

            node *arr = (node *)coll->collection;

            for(size_t i = 0; i < coll->length; i++) {
                traverse_node(&arr[i], depth+2);
            }

            for (int i = 0; i < depth; i++) printf("  ");
            printf("]\n");

            break;
        default:
            printf("UNKNOWN NODE\n");
    }
}


node *parse(scan_result *sr) {
    if(!sr || !sr->tokens || sr->token_count == 0) {
        fprintf(stderr, "Cannot parse the given scan_result struct.\n");
        return NULL;
    }

    current = 0;
    stream = sr->tokens;

    node n;
    if(parse_value(&n) != 0) {
        return NULL;
    }

    // traverse everything and print here.
    traverse_node(&n, 0);
    return NULL;
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
            n->value = malloc(sizeof(t.value));

            if(!n->value) {
                perror("Failed to allocate memory for node value");
                return -1;
            }

            strcpy(n->value, t.value);
            return 0;
        case NUMBER:
        case LT_TRUE:
        case LT_FALSE:
            int is_float = strchr(t.value, '.') != NULL;
            n->type = t.type == NUMBER ? (is_float ? NODE_NUMBER_FLOAT : NODE_NUMBER_INT) : NODE_BOOLEAN;
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
    n->value = NULL;
    kv_pair *coll = NULL;
    size_t i = 0;

    token t = stream[++current]; // move past '{'

    while(t.type != END_OBJECT) {
        if(t.type == STRING) {
            kv_pair pair;
            strcpy(pair.key, t.value);

            t = stream[++current];

            if(t.type != NAME_SEPARATOR) {
                fprintf(stderr, "Expected ':' at position %ld, but didn't find it.\n", current-1);
                return -1;
            }

            current++;

            pair.value = malloc(sizeof(node));
            if(parse_value((node *)pair.value) != 0) {
                return -1;
            }

            coll = realloc(coll, sizeof(kv_pair)*(i+1));
            coll[i++] = pair;
        } else {
            fprintf(stderr, "Expected object key at position %ld.\n", current-1);
            return -1;
        }

        t = stream[++current];
        if(t.type == VALUE_SEPARATOR) {
            t = stream[++current];

            if(t.type == END_OBJECT) {
                fprintf(stderr, "Expected object key at position %ld.\n", current-1);
                return -1;
            }
        }
    }

    n->value = malloc(sizeof(collection));
    ((collection*)n->value)->collection = (void *)coll;
    ((collection*)n->value)->length = i;

    return 0;
}

static int parse_array(node *n) {
    if(!n) {
        fprintf(stderr, "Invalid pointer to node struct given.\n");
        return -1;
    }

    n->type = NODE_ARRAY;
    n->value = NULL;

    node *coll = NULL;
    size_t i = 0;
    size_t length = 0;

    token t = stream[++current]; // move past '['

    while(t.type != END_ARRAY) {
        if(t.type == VALUE_SEPARATOR) {
            if(i % 2 != 1) {
                fprintf(stderr, "Unexpected value seperator at position %ld.\n", current-1);
                return -1;
            }

            // move on to the next token
            t = stream[++current];

            // there should be a new value
            if(t.type == END_ARRAY) {
                fprintf(stderr, "Expected value at position %ld, got end of array.\n", current-1);
                return -1;
            }

            i++;
        } else {
            if(i % 2 != 0) {
                fprintf(stderr, "Unexpected value at position %ld, expected value seperator.\n", current-1);
                return -1;
            }

            node arr_val;

            if(parse_value(&arr_val) != 0) {
                return -1;
            }

            coll = realloc(coll, sizeof(node)*(length+1));
            coll[length++] = arr_val;
            t = stream[++current];

            i++;
        }
    }

    n->value = malloc(sizeof(collection));
    ((collection *)n->value)->collection = (void *)coll;
    ((collection *)n->value)->length = length;

    return 0;
}
