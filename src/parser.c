#include "include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static token *next_token(token *t_stream);

static int parse_value(node *n);
static int parse_object(node *n);

static size_t current;
static token *stream = NULL;

// written by gpt bc I'm too lazy to finish the night by writing my own traversing function.
static void traverse_node(node *n, int depth) {
    if (!n) return;

    // indent
    for (int i = 0; i < depth; i++) printf("  ");

    switch (n->type) {
        case NODE_STRING:
            printf("STRING: %s\n", (char *)n->value);
            break;

        case NODE_NUMBER:
            printf("NUMBER: %d\n", *(int *)n->value);
            break;

        case NODE_BOOLEAN:
            printf("BOOLEAN: %s\n", *(int *)n->value ? "true" : "false");
            break;

        case NODE_NULL:
            printf("NULL\n");
            break;

        case NODE_OBJECT: {
            printf("OBJECT {\n");

            kv_pair *pairs = (kv_pair *)n->value;
            size_t i = 0;

            while (pairs && pairs[i].key[0] != '\0') {
                for (int j = 0; j < depth + 1; j++) printf("  ");
                printf("KEY: %s\n", pairs[i].key);

                traverse_node(pairs[i].value, depth + 2);
                i++;
            }

            for (int i = 0; i < depth; i++) printf("  ");
            printf("}\n");
            break;
        }

        case NODE_ARRAY:
            printf("ARRAY (not implemented)\n");
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
    parse_value(&n);

    printf("NODE TYPE: %d\n", n.type);

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
            n->type = t.type == NUMBER ? NODE_NUMBER : NODE_BOOLEAN;
            n->value = malloc(sizeof(int));

            if(!n->value) {
                perror("Failed to allocate memory for node value");
                return -1;
            }

            *(int *)n->value = atoi(t.value);

            return 0;
        case LT_NULL:
            n->type = NODE_NULL;
            n->value = NULL;
            return 0;
        case BEGIN_OBJECT:
            return parse_object(n);
        case BEGIN_ARRAY:
            return 0;
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

            n->value = realloc(n->value, sizeof(kv_pair)*(i+1));
            ((kv_pair *)n->value)[i++] = pair;
        } else {
            fprintf(stderr, "Expected String.\n");
            return -1;
        }

        t = stream[++current];
        if(t.type == VALUE_SEPARATOR) t = stream[++current];
    }

    return 0;
}
