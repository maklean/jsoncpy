#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

#define ARENA_DEFAULT_BLOCK_SIZE 256
#define ARENA_DEFAULT_ALIGNMENT (sizeof(void *) * 2) // 16 bytes on 64 bit systems, 8 bytes on 32 bit systems

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct ArenaBlock ArenaBlock;

struct ArenaBlock {
    unsigned char* buf;
    size_t buf_len; // total bytes in buffer
    size_t prev_offset; // relative starting index in buf of the latest allocation
    size_t curr_offset; // offset ptr into buffer (also, relative ending index in buf of the latest allocation)

    ArenaBlock* prev; // the previous arena block before this one
};

// Initializes an arena with a buffer size of `max(ARENA_BLOCK_DEFAULT_SIZE, buf_len)`.
void arena_init(ArenaBlock** block, size_t buf_len);

// Allocates memory on the arena and returns the pointer to the start of the memory.
void* arena_alloc(ArenaBlock** block, size_t size);

// Resizes memory allocated on the arena from `old_size` to `new_size`. Returns the pointer to the new memory.
void* arena_resize(ArenaBlock** block, void* old_memory, size_t old_size, size_t new_size);

// Frees the entire arena from the heap.
void arena_clean(ArenaBlock** block);

#endif