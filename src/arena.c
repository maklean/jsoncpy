#include "include/arena.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// Checks whether the given address is a power of two.
static bool is_power_of_two(uintptr_t x);

// Aligns the given pointer to a multiple of the specified alignment.
static uintptr_t align_ptr_forward(uintptr_t ptr, uintptr_t align);

// Allocates memory on the arena starting at a relative (to the buffer) address which is multiple of the specified alignment.
static void *arena_alloc_align(Arena *a, size_t size, size_t align);

// Resizes an item in the arena to 'new_size', the new memory block will start at a multiple of the specified alignment.
static void *arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size, size_t align);

Arena *arena_init(size_t buf_len) {
    if(buf_len == 0) {
        return NULL;
    }

    Arena *a = (Arena *)calloc(1, sizeof(Arena));
    if(!a) {
        return NULL;
    }

    a->buf = (unsigned char *)calloc(1, buf_len);
    if(!a->buf) {
        return NULL;
    }
    a->buf_len = buf_len;

    return a;
}

void *arena_alloc(Arena *a, size_t size) {
    if(!a) {
        return NULL;
    }

    return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void *arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size) {
    if(!a) {
        return NULL;
    }

    return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}

int arena_reset(Arena *a) {
    if(!a) {
        return -1;
    }

    a->prev_offset = 0;
    a->curr_offset = 0;

    return 0;
}

int arena_free(Arena *a) {
    if(!a) {
        return -1;
    }

    if(a->buf) {
        free(a->buf);
    }

    free(a);

    return 0;
}

static void *arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size, size_t align) {
    assert(is_power_of_two(align));

    // trying to allocate new memory
    if(old_memory == NULL || old_size == 0) {
        return arena_alloc_align(a, new_size, align);
    }

    unsigned char *old_mem = (unsigned char *)old_memory; // convert to byte ptr

    // old_memory address is not in the arena
    if(old_mem < a->buf || a->buf + a->buf_len <= old_mem) {
        return NULL;
    }

    // extending or shrinking the latest allocation in the arena
    if(a->buf + a->prev_offset == old_mem) {
        a->curr_offset = a->prev_offset + new_size;

        // zero new memory if we're specifically extending
        if(new_size > old_size) {
            memset(&a->buf[a->prev_offset + old_size], 0, new_size-old_size);
        }

        return old_memory;
    }

    // resizing an arbitrary block in the arena
    void *new_memory = arena_alloc_align(a, new_size, align);
    if(!new_memory) {
        return  NULL;
    }

    /*
        extending -> copy entire old memory
        shrinking -> copy part of old memory

        we can get the size we need to copy by getting: min(old_size, new_size)
    */
    size_t copy_size = old_size < new_size ? old_size : new_size;

    memmove(new_memory, old_memory, copy_size);

    return new_memory;
}

static void *arena_alloc_align(Arena *a, size_t size, size_t align) {
    // get aligned offset to start of new memory block
    uintptr_t aligned_offset = align_ptr_forward((uintptr_t)a->buf + (uintptr_t)a->curr_offset, align);    
    if(!aligned_offset) {
        return NULL;
    }

    aligned_offset -= (uintptr_t)a->buf; // get relative offset from a->buf

    // not enough space in arena
    if(aligned_offset+size > a->buf_len) {
        return NULL;
    }

    void *ptr = &a->buf[aligned_offset];

    a->prev_offset = aligned_offset;
    a->curr_offset = a->prev_offset + size;

    memset(ptr, 0, size);

    return ptr;
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