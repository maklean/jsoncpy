#ifndef PARSER_H
#define PARSER_H

#include "types.h"

// parses a series of tokens into an AST
node *parse(scan_result *sr);

#endif