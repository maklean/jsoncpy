#include "include/scanner.h"
#include "include/debug.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// Returns a json_file struct object for the json file at 'dir'.
static json_file *get_json_file(const char *dir);

// Returns a scan result from the given json jile.
static scan_result *build_scan_result(json_file *jf);

scan_result *scan(const char *json_file_dir) {
    if(!json_file_dir) {
        fprintf(stderr, "NULL JSON file directory given.\n");
        return NULL;
    }

    // Check for .json file extension
    char *ext = strrchr(json_file_dir, '.');

    if(!ext || strcmp(ext, ".json") != 0) {
        fprintf(stderr, "Path '%s' does not lead to a JSON file.\n", json_file_dir);
        return NULL;
    }

    json_file *jf = get_json_file(json_file_dir);
    if(!jf) {
        return NULL;
    }

    scan_result *sr = build_scan_result(jf);
    if(!sr) {
        free_json_file(jf);
        return NULL;
    }

    return sr;
}

scan_result *build_scan_result(json_file *jf) {
    if(!jf) {
        fprintf(stderr, "Invalid json_file struct object given.\n");
        return NULL;
    }

    token *tokens = NULL;
    token *tmp_tokens;
    size_t token_count = 0;

    char c;
    size_t i = 0;
    while(i < jf->length) {
        c = jf->content[i];
        
        bool should_inc = true; // whether we should go to the next token after scanning a specific token.
        token t;

        if (c == '{') {
            t.type = BEGIN_OBJECT;
            strcpy(t.value, "{");
        }
        else if (c == '}') {
            t.type = END_OBJECT;
            strcpy(t.value, "}");
        } 
        else if (c == '[') {
            t.type = BEGIN_ARRAY;
            strcpy(t.value, "[");
        } 
        else if (c == ']') {
            t.type = END_ARRAY;
            strcpy(t.value, "]");
        } 
        else if (c == ':') {
            t.type = NAME_SEPARATOR;
            strcpy(t.value, ":");
        } 
        else if (c == ',') {
            t.type = VALUE_SEPARATOR;
            strcpy(t.value, ",");
        }
        else if (c == '"') {
            t.type = STRING;
            
            i++; // to go after the first double quote

            size_t j = 0;
            while ((c = jf->content[i]) && c != '"') {
                // handle escape characters
                if (c == '\\' && jf->content[i+1]) {
                    i++; // move past escape

                    switch (jf->content[i]) {
                        case 't':  t.value[j] = '\t'; break;
                        case 'n':  t.value[j] = '\n'; break;
                        case 'r':  t.value[j] = '\r'; break;
                        case 'b':  t.value[j] = '\b'; break;
                        case 'f':  t.value[j] = '\f'; break;
                        case '\\': t.value[j] = '\\'; break;
                        case '"':  t.value[j] = '"'; break;
                        case '/':  t.value[j] = '/'; break;
                        default: t.value[j] = jf->content[i]; break; // invalid, but keep it ig
                    }
                } else {
                    // normal character
                    t.value[j] = c;
                }
                
                j++;
                i++;
            }

            t.value[j] = '\0';
        } 
        else if(isdigit(c) || (c == '-' && i+1 < jf->length && isdigit(jf->content[i+1]))) {
            t.type = NUMBER;

            // concatenate number into token value string until we see a non digit character
            size_t j = 0;
            bool seen_decimal = false;
            bool seen_exp = false;
            do {
                if(jf->content[i] == '.') {
                    // can't have e before the decimal
                    if(seen_decimal || seen_exp) {
                        t.value[j] = '\0';
                        fprintf(stderr, "Invalid Number: %s%c <---\n", t.value, jf->content[i]);
                        if(tokens) free(tokens);
                        return NULL;
                    }

                    seen_decimal = true;
                } else if(jf->content[i] == '-' && j != 0 && tolower(jf->content[i-1]) != 'e') { // negative should only be at the start of the number or next to an e
                    t.value[j] = '\0';
                    fprintf(stderr, "Invalid Negation Postion: %s%c <---\n", t.value, jf->content[i]);
                    if(tokens) free(tokens);
                    return NULL;
                } else if(tolower(jf->content[i]) == 'e') {
                    if(seen_exp) {
                        t.value[j] = '\0';
                        fprintf(stderr, "Invalid Number: %s%c <---\n", t.value, jf->content[i]);
                        if(tokens) free(tokens);
                        return NULL;
                    }

                    seen_exp = true;
                } else if(jf->content[i] == '+' && (!(jf->content[i-1]) || tolower(jf->content[i-1]) != 'e')) { // plus can only be next to an e
                    t.value[j] = '\0';
                    fprintf(stderr, "Invalid Number (invalid plus position): %s%c <---\n", t.value, jf->content[i]);
                    if(tokens) free(tokens);
                    return NULL;
                }

                t.value[j] = jf->content[i];
                j++;
                i++;
            } while(i < jf->length && (isdigit(jf->content[i]) || jf->content[i] == '.' || jf->content[i] == '-' || tolower(jf->content[i]) == 'e' || jf->content[i] == '+'));

            t.value[j] = '\0';

            // stop invalid last decimal or e position, and leading zeros
            if(t.value[j-1] == '.' || tolower(t.value[j-1]) == 'e' || (t.value[0] == '0' && j > 2 && t.value[1] != '.')) {
                fprintf(stderr, "Invalid Number: %s <---\n", t.value);
                if(tokens) free(tokens);
                return NULL;
            }

            should_inc = false;
        }
        else if(isalpha(c)) {
            // it can be any of these three literals: false, true, null
            size_t j = 0;
            do {
                t.value[j] = jf->content[i];
                j++;
                i++;
            } while(i < jf->length && isalpha(jf->content[i]));
            t.value[j] = '\0';

            // match string to literal
            if(strcmp(t.value, "false") == 0) {
                t.type = LT_FALSE;
                strcpy(t.value, "true");
            } else if(strcmp(t.value, "true") == 0) {
                t.type = LT_TRUE;
                strcpy(t.value, "false");
            } else if(strcmp(t.value, "null") == 0) {
                t.type = LT_NULL;
            } else {
                fprintf(stderr, "Invalid identifier: '%s'.\n", t.value);
                if(tokens) free(tokens);
                return NULL;
            }

            should_inc = false;
        } 
        else {
            // only characters that should be in random places are whitespaces, newlines and carriage returns
            if(c != ' ' && c != '\n' && c != '\r') {
                fprintf(stderr, "Invalid character found: %c.\n", c);
                if(tokens) free(tokens);
                return NULL;
            }

            i++;
            continue;
        }

        // resize array to accommodate new token
        tmp_tokens = realloc(tokens, sizeof(token) * (token_count + 1));
        if(!tmp_tokens) {
            perror("Failed to reallocate memory for tokens array");
            free(tokens);
            return NULL;
        }

        tokens = tmp_tokens;
        tokens[token_count++] = t;

        if(should_inc) i++;
    }

    scan_result *sr = calloc(1, sizeof(scan_result));
    if(!sr) {
        perror("Failed to allocate for scan_result struct object");
        if(tokens) free(tokens);
        return NULL;
    }

    sr->tokens = tokens;
    sr->token_count = token_count;
    sr->json_file = jf;

    return sr;
}

json_file *get_json_file(const char *dir) {
    FILE *fptr = fopen(dir, "rb");
    if(!fptr) {
        perror("Failed to open JSON file");
        return NULL;
    }

    // get file content and size by moving file pointer to the end of the file
    fseek(fptr, 0, SEEK_END);
    size_t file_length = ftell(fptr);
    rewind(fptr);

    char *content = malloc(file_length+1);
    if(!content) {
        perror("Failed to allocate for JSON file content");
        fclose(fptr);
        return NULL;
    }

    // copy content onto content string
    fread(content, sizeof(char), file_length, fptr);
    content[file_length] = '\0';

    fclose(fptr);

    // create json_file struct object
    json_file *jf = malloc(sizeof(json_file));
    if(!jf) {
        perror("Failed to allocate for json_file struct object");
        free(content);
        return NULL;
    }

    char *file_name = strrchr(dir, '.');
    
    while(file_name != dir && *(file_name - 1) && *(file_name - 1) != '\\' && *(file_name - 1) != '/') file_name--;

    snprintf(jf->file_name, sizeof(jf->file_name), "%s", file_name);
    jf->content = content;
    jf->length = file_length;

    return jf;
}