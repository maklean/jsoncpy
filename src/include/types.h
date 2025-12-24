#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

// Follows the JSON Grammar defined in RFC8259 (Source: https://datatracker.ietf.org/doc/html/rfc8259#section-2)
typedef enum token_type_e {
    UNKNOWN,
    BEGIN_ARRAY,
    END_ARRAY,
    BEGIN_OBJECT,
    END_OBJECT,
    NAME_SEPERATOR,
    VALUE_SEPERATOR,
    LT_FALSE,
    LT_TRUE,
    LT_NULL,
    STRING,
    NUMBER
} token_type;

typedef struct token_s {
    token_type type;
    char value[513]; // 512 bytes for each token value for now...
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

#endif