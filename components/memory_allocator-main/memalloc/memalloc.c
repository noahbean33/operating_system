#include "memalloc.h"
#include <string.h>
#include <windows.h>

typedef char ALIGN[16];

union header {
    struct {
        size_t size;
        unsigned is_free;
        union header *next;
    } s;
    ALIGN stub;
};
typedef union header header_t;

header_t *head = NULL, *tail = NULL;
CRITICAL_SECTION global_malloc_lock;

static int malloc_initialized = 0;

static void initialize_malloc() {
    if (!malloc_initialized) {
        InitializeCriticalSection(&global_malloc_lock);
        malloc_initialized = 1;
    }
}

header_t *get_free_block(size_t size) {
    header_t *curr = head;
    while (curr) {
        if (curr->s.is_free && curr->s.size >= size)
            return curr;
        curr = curr->s.next;
    }
    return NULL;
}

void *malloc(size_t size) {
    if (!malloc_initialized) initialize_malloc();
    
    if (!size) return NULL;
    
    EnterCriticalSection(&global_malloc_lock);
    
    header_t *header = get_free_block(size);
    if (header) {
        header->s.is_free = 0;
        LeaveCriticalSection(&global_malloc_lock);
        return (void*)(header + 1);
    }
    
    size_t total_size = sizeof(header_t) + size;
    void *block = VirtualAlloc(NULL, total_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (block == NULL) {
        LeaveCriticalSection(&global_malloc_lock);
        return NULL;
    }
    
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    
    if (!head) head = header;
    if (tail) tail->s.next = header;
    tail = header;
    
    LeaveCriticalSection(&global_malloc_lock);
    return (void*)(header + 1);
}

void free(void *block) {
    if (!block) return;
    
    if (!malloc_initialized) initialize_malloc();
    
    EnterCriticalSection(&global_malloc_lock);
    
    header_t *header = (header_t*)block - 1;
    
    if (header == tail) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            header_t *tmp = head;
            while (tmp) {
                if (tmp->s.next == tail) {
                    tmp->s.next = NULL;
                    tail = tmp;
                    break;
                }
                tmp = tmp->s.next;
            }
        }
        
        VirtualFree(header, 0, MEM_RELEASE);
    } else {
        header->s.is_free = 1;
    }
    
    LeaveCriticalSection(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize) {
    size_t size;
    void *block;
    
    if (!num || !nsize) return NULL;
    
    size = num * nsize;
    if (nsize != size / num) return NULL;
    
    block = malloc(size);
    if (!block) return NULL;
    
    memset(block, 0, size);
    return block;
}

void *realloc(void *block, size_t size) {
    if (!block || !size) return malloc(size);
    
    header_t *header = (header_t*)block - 1;
    if (header->s.size >= size) return block;
    
    void *ret = malloc(size);
    if (ret) {
        memcpy(ret, block, header->s.size);
        free(block);
    }
    return ret;
}
