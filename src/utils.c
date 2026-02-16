#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>

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