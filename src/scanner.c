#include "include/scanner.h"
#include "include/debug.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Returns a json_file struct object for the json file at 'dir'.
static json_file *get_json_file(const char *dir);

// Returns an array of tokens for the given json file.
static token *fetch_tokens(json_file *jf);

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

    scan_result *sr = calloc(1, sizeof(scan_result));
    if(!sr) {
        perror("Failed to allocate for scan_result struct object");
        free(jf->content);
        free(jf);
        return NULL;
    }

    sr->tokens = fetch_tokens(jf);

    sr->json_file = jf;

    return sr;
}

token *fetch_tokens(json_file *jf) {
    if(!jf) {
        fprintf(stderr, "Invalid json_file struct object given.\n");
        return NULL;
    }

    token *tokens = NULL;
    size_t token_size = 0;

    char c;
    size_t i = 0;
    while(i < jf->length) {
        c = jf->content[i];
        
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
            t.type = NAME_SEPERATOR;
            strcpy(t.value, ":");
        } 
        else if (c == ',') {
            t.type = VALUE_SEPERATOR;
            strcpy(t.value, ",");
        }
        else if (c == '"') {
            t.type = STRING;
            
            i++;
            size_t j = 0;
            while(i < jf->length && (c = jf->content[i]) != '"') {
                t.value[j] = c;
                j++;
                i++;
            }
            t.value[j] = '\0';
        } else {
            // whitespace (or the other tokens I'm not processing atm)
            i++;
            continue;
        }

        tokens = realloc(tokens, sizeof(token) * (++token_size));
        if(!tokens) {
            return NULL;
        }
        tokens[token_size-1] = t;

        printf("\nType: %d\nValue: '%s'\n", t.type, t.value);

        i++;
    }

    return tokens;
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