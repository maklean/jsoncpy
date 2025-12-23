#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>

void free_scan_result(scan_result *sr) {
    if(!sr) {
        fprintf(stderr, "Invalid scan_result struct object given.\n");
        return;
    }

    if(sr->json_file) free_json_file(sr->json_file);
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