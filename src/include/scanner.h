#ifndef SCANNER_H
#define SCANNER_H

#include "types.h"

/**
 * Scans through the contents of the provided JSON file and returns the result of the scan.
 * 
 * @returns A `scan_result` struct object.
 */
scan_result *scan(const char *json_file_dir);

#endif