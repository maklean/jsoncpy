#ifndef UTILS_H
#define UTILS_H

#include "types.h"

void free_scan_result(scan_result *sr);

void free_json_file(json_file *jf);

const char *token_type_to_str(token_type t);

#endif