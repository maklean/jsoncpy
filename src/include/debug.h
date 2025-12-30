#ifndef DEBUG_H
#define DEBUG_H

#include "types.h"

// Prints the information in a json_file struct object (for debugging purposes)
void debug_json_file(json_file *jf);

// Prints the information in a scan_result struct object (for debugging purposes)
void debug_scan_result(scan_result *sr);

// Prints the information in a node struct object (for debugging purposes)
void debug_node_tree(node *n, int depth);

#endif