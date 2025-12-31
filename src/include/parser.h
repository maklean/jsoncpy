#ifndef PARSER_H
#define PARSER_H

#include "types.h"

// Parses a series of tokens into an AST. Returns the root of the AST (i.e. NODE_OBJECT).
node *parse(scan_result *sr);

#endif