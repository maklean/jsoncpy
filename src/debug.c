#include "include/debug.h"
#include <stdio.h>

void debug_json_file(json_file *jf) {
    if(!jf) {
        fprintf(stderr, "Invalid json_file struct given.\n");
        return;
    }

    // the pointer information is kinda useless for me, but it makes it look like I know what I'm doing so why the hell not.
    printf("=== json_file (Address: %p) ===:\nName: %s\nLength: %ld\nContent: %s\n", jf, jf->file_name, jf->length, jf->content);
}

void debug_scan_result(scan_result *sr) {
    if(!sr) {
        fprintf(stderr, "Invalid scan_result struct given.\n");
        return;
    }

    // lowk, TBC...
    printf("=== scan_result (Address: %p) ===:\nToken Count: %ld\nTokens: []", sr, sr->token_count);
}