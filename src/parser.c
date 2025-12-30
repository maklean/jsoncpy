#include "include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static int parse_value(node *n);
static int parse_object(node *n);
static int parse_array(node *n);

static size_t current;
static token *stream = NULL;

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
