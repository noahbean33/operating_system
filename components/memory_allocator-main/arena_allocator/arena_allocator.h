#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ARENA_KILOBYTES(n) ((size_t)(n) << 10)
#define ARENA_MEGABYTES(n) ((size_t)(n) << 20)
#define ARENA_GIGABYTES(n) ((size_t)(n) << 30)

typedef struct arena_allocator {
    size_t reserve_size;
    size_t commit_size;
    size_t position;
    size_t commit_position;
} arena_allocator_t;

arena_allocator_t* arena_create(size_t reserve_size, size_t commit_size);
void arena_destroy(arena_allocator_t* arena);
void* arena_alloc(arena_allocator_t* arena, size_t size);
void* arena_alloc_aligned(arena_allocator_t* arena, size_t size, size_t alignment);
void arena_reset(arena_allocator_t* arena);
size_t arena_get_position(const arena_allocator_t* arena);
void arena_set_position(arena_allocator_t* arena, size_t position);

#endif
