#ifndef TYPES_H
#define TYPES_H

#define VALUE_STR_BUFFER 513 // 512 characters + 1 for null-terminator

#include <stddef.h>

// Follows the JSON Grammar defined in RFC8259 (Source: https://datatracker.ietf.org/doc/html/rfc8259#section-2)
typedef enum token_type_e {
    BEGIN_ARRAY,
    END_ARRAY,
    BEGIN_OBJECT,
    END_OBJECT,
    NAME_SEPARATOR,
    VALUE_SEPARATOR,
    LT_FALSE,
    LT_TRUE,
    LT_NULL,
    STRING,
    NUMBER
} token_type;

typedef enum node_type_e {
    NODE_OBJECT,
    NODE_ARRAY,
    NODE_STRING,
    NODE_NUMBER_INT,
    NODE_NUMBER_FLOAT,
    NODE_BOOLEAN,
    NODE_NULL
} node_type;

typedef struct token_s {
    token_type type;
    char value[VALUE_STR_BUFFER];
} token;

typedef struct json_file_s {
    char file_name[256];
    char *content;
    size_t length;
} json_file;

typedef struct scan_result_s {
    json_file *json_file;
    token *tokens;
    size_t token_count;
} scan_result;

typedef struct node_s {
    node_type type;
    void *value;
} node;

typedef struct kv_pair_s {
    char key[VALUE_STR_BUFFER];
    node *value;
} kv_pair;

typedef struct collection_s {
    void *collection;
    size_t length;
} collection;

#endif