#include "include/arena.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

// Checks whether the given address is a power of two.
static bool is_power_of_two(uintptr_t x);

// Aligns the given pointer to a multiple of the specified alignment.
static uintptr_t align_ptr_forward(uintptr_t ptr, uintptr_t align);

// Allocates memory on the arena starting at a relative (to the buffer) address which is multiple of the specified alignment.
static void* arena_alloc_align(ArenaBlock** block, size_t size, size_t alignment);

// Resizes an item in the arena to 'new_size', the new memory block will start at a multiple of the specified alignment.
static void *arena_resize_align(ArenaBlock** block, void *old_memory, size_t old_size, size_t new_size, size_t alignment);

// Returns whether the given address is on any of the arena blocks.
static bool arena_address_is_on_block(ArenaBlock* block, void* ptr);

void arena_init(ArenaBlock** block, size_t buf_len) {
    if(block == NULL) {
        return;
    }

    size_t buf_size = MAX(ARENA_DEFAULT_BLOCK_SIZE, buf_len);

    // calloc() to zero the other members
    ArenaBlock* blk = (ArenaBlock*)calloc(1, sizeof(ArenaBlock));
    if(blk == NULL) {
        *block = NULL;
        return;
    }

    blk->buf = (unsigned char*)malloc(buf_size);
    if(blk->buf == NULL) {
        free(blk);
        *block = NULL;
        return;
    }

    blk->buf_len = buf_size;
    *block = blk;
}

void* arena_alloc(ArenaBlock** block, size_t size) {
    if(block == NULL || *block == NULL || size == 0) {
        return NULL;
    }

    return arena_alloc_align(block, size, ARENA_DEFAULT_ALIGNMENT);
}

static void* arena_alloc_align(ArenaBlock** block, size_t size, size_t alignment) {
    ArenaBlock* blk = *block;

    uintptr_t aligned_offset = align_ptr_forward((uintptr_t)blk->buf + (uintptr_t)blk->curr_offset, (uintptr_t)alignment);
    if(!aligned_offset) {
        return NULL;
    }

    aligned_offset -= (uintptr_t)blk->buf;

    // not enough memory on current block, allocate new block
    if(aligned_offset+size > (uintptr_t)blk->buf_len) {
        ArenaBlock* new_blk = NULL;

        arena_init(&new_blk, size + alignment - 1);
        if(new_blk == NULL) {
            return NULL;
        }

        void* ptr = arena_alloc_align(&new_blk, size, alignment);
        if(ptr == NULL) {
            // clean new block if couldn't allocate memory
            arena_clean(&new_blk);
            return NULL;
        }

        // add new block to chain
        new_blk->prev = blk;
        *block = new_blk;

        return ptr;
    }

    void* ptr = &blk->buf[aligned_offset];

    blk->prev_offset = aligned_offset;
    blk->curr_offset = aligned_offset+size;

    memset(ptr, 0, size);

    return ptr;
}

void* arena_resize(ArenaBlock** block, void* old_memory, size_t old_size, size_t new_size) {
    if(block == NULL || *block == NULL || new_size == 0) {
        return NULL;
    }

    return arena_resize_align(block, old_memory, old_size, new_size, ARENA_DEFAULT_ALIGNMENT);
}

static void *arena_resize_align(ArenaBlock** block, void *old_memory, size_t old_size, size_t new_size, size_t alignment) {
    if(old_memory == NULL || old_size == 0) {
        return arena_alloc_align(block, new_size, alignment);
    }

    ArenaBlock* blk = *block;
    unsigned char* old_mem = (unsigned char*)old_memory;

    // old memory is not in the arena
    if(!arena_address_is_on_block(*block, old_memory)) {
        return NULL;
    }

    // extending or shrinking the latest allocation in the arena
    if(blk->buf+blk->prev_offset == old_mem) {
        // zero new memory if we're specifically extending
        if(new_size > old_size) {
            // not enough space on this block for extension, allocate on new block
            if(blk->prev_offset+new_size > blk->buf_len) {
                ArenaBlock* new_blk = NULL;
                arena_init(&new_blk, new_size + alignment - 1);

                if(new_blk == NULL) {
                    return NULL;
                }

                void* ptr = arena_alloc_align(&new_blk, new_size, alignment);
                if(ptr == NULL) {
                    arena_clean(&new_blk);
                    return NULL;
                }

                // copy memory over to new block
                memmove(ptr, old_memory, old_size);

                // connect blocks
                new_blk->prev = blk;
                *block = new_blk;

                return ptr;
            }

            // zero new memory if we're extending and there's enough space
            memset(&blk->buf[blk->prev_offset+old_size], 0, new_size-old_size);
        }

        blk->curr_offset = blk->prev_offset+new_size;
        return old_memory;
    }

    // resizing arbitrary block in arena
    void* new_memory = arena_alloc_align(block, new_size, alignment);
    if(new_memory == NULL) {
        return NULL;
    }

    size_t copy_size = MIN(old_size, new_size);
    memmove(new_memory, old_memory, copy_size);

    return new_memory;
}

static bool arena_address_is_on_block(ArenaBlock* block, void* ptr) {
    if(block == NULL || ptr == NULL) {
        return false;
    }

    ArenaBlock* curr = block;

    while(curr != NULL) {
        if(curr->buf <= (unsigned char*)ptr && (unsigned char*)ptr < curr->buf + curr->buf_len) {
            return true;
        }

        curr = curr->prev;
    }

    return false;
}

void arena_clean(ArenaBlock** block) {
    if(block == NULL || *block == NULL) {
        return;
    }

    ArenaBlock* blk = *block;
    ArenaBlock* tmp;

    // traverse list (backwards), free every block
    while(blk != NULL) {
        tmp = blk->prev;

        free(blk->buf);
        free(blk);

        blk = tmp;
    }

    *block = NULL;
}

static uintptr_t align_ptr_forward(uintptr_t ptr, uintptr_t align) {
    // alignment must be a power of two (virtually all architectures support this)
    assert(is_power_of_two(align));

    uintptr_t remainder = ptr & (align-1); // faster way of doing p % a

    // align to next multiple of 'align' if there is a remainder
    if(remainder != 0) {
        ptr += align - remainder;
    }

    return ptr;
}

static bool is_power_of_two(uintptr_t x) {
    // powers of two should give 0b0000 when subtracted by x-1
    return (x & (x-1)) == 0;
}
