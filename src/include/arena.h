#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

#define DEFAULT_ALIGNMENT (sizeof(void *) * 2) // 16 bytes on 64 bit systems, 8 bytes on 32 bit systems
#define MAX_ARENA_SIZE_MB 100 // 100 MB (TODO: add dynamic resizing to arena implementation)

typedef struct Arena {
    unsigned char *buf;
    size_t buf_len; // total bytes in buffer
    size_t prev_offset; // relative starting index of the latest allocation
    size_t curr_offset; // offset ptr into buffer (also, relative ending index of the latest allocation)
} Arena;

// Creates an arena with a total size of 'buf_len'
Arena *arena_init(size_t buf_len);

// Allocates 'size' bytes on the arena and returns the pointer to the start of the allocated memory.
void *arena_alloc(Arena *a, size_t size);

// Resizes an item on the arena.
void *arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size);

// Resets the entire arena (basically, zeros the offsets)
int arena_reset(Arena *a);

// Deallocates (frees) the entire arena.
int arena_free(Arena *a);

#endif