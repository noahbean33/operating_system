#include "arena_allocator.h"
#include <string.h>
#include <stdlib.h>

#define ARENA_HEADER_SIZE (sizeof(arena_allocator_t))
#define DEFAULT_ALIGNMENT (sizeof(void*))

static inline size_t align_up(size_t value, size_t alignment) {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

static inline size_t min_size(size_t a, size_t b) {
    return (a < b) ? a : b;
}

static uint32_t platform_get_page_size(void);
static void* platform_reserve_memory(size_t size);
static bool platform_commit_memory(void* ptr, size_t size);
static bool platform_release_memory(void* ptr, size_t size);

arena_allocator_t* arena_create(size_t reserve_size, size_t commit_size) {
    if (reserve_size == 0 || commit_size == 0) {
        return NULL;
    }

    uint32_t page_size = platform_get_page_size();
    reserve_size = align_up(reserve_size, page_size);
    commit_size = align_up(commit_size, page_size);

    if (commit_size > reserve_size) {
        commit_size = reserve_size;
    }

    arena_allocator_t* arena = (arena_allocator_t*)platform_reserve_memory(reserve_size);
    if (arena == NULL) {
        return NULL;
    }

    if (!platform_commit_memory(arena, commit_size)) {
        platform_release_memory(arena, reserve_size);
        return NULL;
    }

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->position = ARENA_HEADER_SIZE;
    arena->commit_position = commit_size;

    return arena;
}

void arena_destroy(arena_allocator_t* arena) {
    if (arena != NULL) {
        platform_release_memory(arena, arena->reserve_size);
    }
}

void* arena_alloc(arena_allocator_t* arena, size_t size) {
    return arena_alloc_aligned(arena, size, DEFAULT_ALIGNMENT);
}

void* arena_alloc_aligned(arena_allocator_t* arena, size_t size, size_t alignment) {
    if (arena == NULL || size == 0) {
        return NULL;
    }

    size_t aligned_position = align_up(arena->position, alignment);
    size_t new_position = aligned_position + size;

    if (new_position > arena->reserve_size) {
        return NULL;
    }

    if (new_position > arena->commit_position) {
        size_t new_commit_position = align_up(new_position, arena->commit_size);
        new_commit_position = min_size(new_commit_position, arena->reserve_size);

        uint8_t* commit_ptr = (uint8_t*)arena + arena->commit_position;
        size_t commit_amount = new_commit_position - arena->commit_position;

        if (!platform_commit_memory(commit_ptr, commit_amount)) {
            return NULL;
        }

        arena->commit_position = new_commit_position;
    }

    arena->position = new_position;

    void* result = (uint8_t*)arena + aligned_position;
    memset(result, 0, size);

    return result;
}

void arena_reset(arena_allocator_t* arena) {
    if (arena != NULL) {
        arena->position = ARENA_HEADER_SIZE;
    }
}

size_t arena_get_position(const arena_allocator_t* arena) {
    return (arena != NULL) ? arena->position : 0;
}

void arena_set_position(arena_allocator_t* arena, size_t position) {
    if (arena != NULL && position >= ARENA_HEADER_SIZE && position <= arena->reserve_size) {
        arena->position = position;
    }
}

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

static uint32_t platform_get_page_size(void) {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    return (uint32_t)system_info.dwPageSize;
}

static void* platform_reserve_memory(size_t size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

static bool platform_commit_memory(void* ptr, size_t size) {
    void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return (result != NULL);
}

static bool platform_release_memory(void* ptr, size_t size) {
    (void)size;
    return VirtualFree(ptr, 0, MEM_RELEASE) != 0;
}

#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)

#define _DEFAULT_SOURCE
#include <unistd.h>
#include <sys/mman.h>

static uint32_t platform_get_page_size(void) {
    long page_size = sysconf(_SC_PAGESIZE);
    return (page_size > 0) ? (uint32_t)page_size : 4096;
}

static void* platform_reserve_memory(size_t size) {
    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (ptr != MAP_FAILED) ? ptr : NULL;
}

static bool platform_commit_memory(void* ptr, size_t size) {
    int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return (result == 0);
}

static bool platform_release_memory(void* ptr, size_t size) {
    int result = munmap(ptr, size);
    return (result == 0);
}

#else
#error "Unsupported platform"
#endif
