#ifndef LOADER_H
#define LOADER_H

/**
 * Loads the JSON file at 'path'.
 * 
 * @returns The JSON file's content.
 * @returns `NULL` If there is no file at 'path', or the file is not a JSON file.
 */
char *load_file(char *path);

#endif