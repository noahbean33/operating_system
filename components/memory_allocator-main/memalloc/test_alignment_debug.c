#include <stdio.h>
#include <stdint.h>
#include "memalloc.h"

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

int main(void) {
    printf("Alignment Analysis\n");
    printf("==================\n\n");
    
    printf("sizeof(header_t): %zu bytes\n", sizeof(header_t));
    printf("alignof(header_t): %zu bytes\n", _Alignof(header_t));
    printf("sizeof(ALIGN): %zu bytes\n", sizeof(ALIGN));
    printf("sizeof(size_t): %zu bytes\n", sizeof(size_t));
    printf("sizeof(unsigned): %zu bytes\n", sizeof(unsigned));
    printf("sizeof(void*): %zu bytes\n", sizeof(void*));
    
    printf("\nTesting actual allocation alignment:\n");
    
    for (int i = 0; i < 5; i++) {
        void* ptr = malloc(1);
        printf("  Allocation %d: %p (mod 16 = %zu)\n", 
               i + 1, ptr, (uintptr_t)ptr % 16);
        free(ptr);
    }
    
    printf("\nTesting multiple simultaneous allocations:\n");
    void* ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(8);
        printf("  Allocation %d: %p (mod 16 = %zu)\n", 
               i + 1, ptrs[i], (uintptr_t)ptrs[i] % 16);
    }
    
    for (int i = 0; i < 5; i++) {
        free(ptrs[i]);
    }
    
    return 0;
}
