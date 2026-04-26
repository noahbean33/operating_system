#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "memalloc.h"

#define TEST_PASSED() printf("  [PASS] %s\n", __func__)
#define TEST_FAILED(msg) printf("  [FAIL] %s: %s\n", __func__, msg)

static int tests_passed = 0;
static int tests_failed = 0;

void test_malloc_basic(void) {
    void* ptr = malloc(100);
    
    if (ptr == NULL) {
        TEST_FAILED("malloc returned NULL for valid size");
        tests_failed++;
        return;
    }
    
    memset(ptr, 0xAA, 100);
    free(ptr);
    
    TEST_PASSED();
    tests_passed++;
}

void test_malloc_zero_size(void) {
    void* ptr = malloc(0);
    
    if (ptr != NULL) {
        TEST_FAILED("malloc should return NULL for zero size");
        tests_failed++;
        free(ptr);
        return;
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_free_null(void) {
    free(NULL);
    
    TEST_PASSED();
    tests_passed++;
}

void test_calloc_basic(void) {
    size_t num = 10;
    size_t size = sizeof(int);
    int* ptr = (int*)calloc(num, size);
    
    if (ptr == NULL) {
        TEST_FAILED("calloc returned NULL");
        tests_failed++;
        return;
    }
    
    for (size_t i = 0; i < num; i++) {
        if (ptr[i] != 0) {
            TEST_FAILED("calloc did not zero initialize memory");
            tests_failed++;
            free(ptr);
            return;
        }
    }
    
    free(ptr);
    TEST_PASSED();
    tests_passed++;
}

void test_calloc_zero_params(void) {
    void* ptr1 = calloc(0, sizeof(int));
    void* ptr2 = calloc(10, 0);
    void* ptr3 = calloc(0, 0);
    
    if (ptr1 != NULL || ptr2 != NULL || ptr3 != NULL) {
        TEST_FAILED("calloc should return NULL for zero parameters");
        tests_failed++;
        if (ptr1) free(ptr1);
        if (ptr2) free(ptr2);
        if (ptr3) free(ptr3);
        return;
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_calloc_overflow(void) {
    size_t num = SIZE_MAX / 2;
    size_t size = 4;
    void* ptr = calloc(num, size);
    
    if (ptr != NULL) {
        TEST_FAILED("calloc should detect overflow");
        tests_failed++;
        free(ptr);
        return;
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_realloc_null_block(void) {
    void* ptr = realloc(NULL, 100);
    
    if (ptr == NULL) {
        TEST_FAILED("realloc with NULL block should act like malloc");
        tests_failed++;
        return;
    }
    
    memset(ptr, 0xBB, 100);
    free(ptr);
    
    TEST_PASSED();
    tests_passed++;
}

void test_realloc_expand(void) {
    char* ptr = (char*)malloc(50);
    if (ptr == NULL) {
        TEST_FAILED("initial malloc failed");
        tests_failed++;
        return;
    }
    
    for (int i = 0; i < 50; i++) {
        ptr[i] = (char)i;
    }
    
    ptr = (char*)realloc(ptr, 100);
    if (ptr == NULL) {
        TEST_FAILED("realloc failed to expand");
        tests_failed++;
        return;
    }
    
    for (int i = 0; i < 50; i++) {
        if (ptr[i] != (char)i) {
            TEST_FAILED("data corrupted during realloc");
            tests_failed++;
            free(ptr);
            return;
        }
    }
    
    free(ptr);
    TEST_PASSED();
    tests_passed++;
}

void test_realloc_shrink(void) {
    char* ptr = (char*)malloc(100);
    if (ptr == NULL) {
        TEST_FAILED("initial malloc failed");
        tests_failed++;
        return;
    }
    
    for (int i = 0; i < 100; i++) {
        ptr[i] = (char)i;
    }
    
    ptr = (char*)realloc(ptr, 50);
    if (ptr == NULL) {
        TEST_FAILED("realloc failed to shrink");
        tests_failed++;
        return;
    }
    
    for (int i = 0; i < 50; i++) {
        if (ptr[i] != (char)i) {
            TEST_FAILED("data corrupted during realloc shrink");
            tests_failed++;
            free(ptr);
            return;
        }
    }
    
    free(ptr);
    TEST_PASSED();
    tests_passed++;
}

void test_multiple_allocations(void) {
    void* ptrs[100];
    
    for (int i = 0; i < 100; i++) {
        ptrs[i] = malloc(64);
        if (ptrs[i] == NULL) {
            TEST_FAILED("allocation failed in loop");
            tests_failed++;
            for (int j = 0; j < i; j++) {
                free(ptrs[j]);
            }
            return;
        }
        memset(ptrs[i], i & 0xFF, 64);
    }
    
    for (int i = 0; i < 100; i++) {
        free(ptrs[i]);
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_allocation_reuse(void) {
    void* ptr1 = malloc(128);
    void* first_address = ptr1;
    
    if (ptr1 == NULL) {
        TEST_FAILED("first allocation failed");
        tests_failed++;
        return;
    }
    
    free(ptr1);
    
    void* ptr2 = malloc(128);
    
    if (ptr2 == NULL) {
        TEST_FAILED("second allocation failed");
        tests_failed++;
        return;
    }
    
    if (ptr2 == first_address) {
        printf("  [INFO] Free list reuse working (same address reused)\n");
    }
    
    free(ptr2);
    TEST_PASSED();
    tests_passed++;
}

void test_large_allocation(void) {
    size_t large_size = 10 * 1024 * 1024;
    void* ptr = malloc(large_size);
    
    if (ptr == NULL) {
        TEST_FAILED("large allocation failed");
        tests_failed++;
        return;
    }
    
    memset(ptr, 0xCC, 1024);
    
    free(ptr);
    TEST_PASSED();
    tests_passed++;
}

void test_alternating_alloc_free(void) {
    for (int i = 0; i < 50; i++) {
        void* ptr = malloc(256);
        if (ptr == NULL) {
            TEST_FAILED("allocation failed in alternating test");
            tests_failed++;
            return;
        }
        memset(ptr, 0xDD, 256);
        free(ptr);
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_mixed_sizes(void) {
    void* small = malloc(8);
    void* medium = malloc(256);
    void* large = malloc(4096);
    
    if (small == NULL || medium == NULL || large == NULL) {
        TEST_FAILED("mixed size allocation failed");
        tests_failed++;
        if (small) free(small);
        if (medium) free(medium);
        if (large) free(large);
        return;
    }
    
    memset(small, 1, 8);
    memset(medium, 2, 256);
    memset(large, 3, 4096);
    
    free(medium);
    
    void* another = malloc(128);
    if (another == NULL) {
        TEST_FAILED("allocation after free failed");
        tests_failed++;
        free(small);
        free(large);
        return;
    }
    
    free(small);
    free(large);
    free(another);
    
    TEST_PASSED();
    tests_passed++;
}

void test_struct_allocation(void) {
    typedef struct {
        int id;
        double value;
        char name[64];
    } TestStruct;
    
    TestStruct* s = (TestStruct*)malloc(sizeof(TestStruct));
    
    if (s == NULL) {
        TEST_FAILED("struct allocation failed");
        tests_failed++;
        return;
    }
    
    s->id = 42;
    s->value = 3.14159;
    strcpy(s->name, "Test Structure");
    
    if (s->id != 42 || s->value != 3.14159) {
        TEST_FAILED("struct data corrupted");
        tests_failed++;
        free(s);
        return;
    }
    
    free(s);
    TEST_PASSED();
    tests_passed++;
}

void test_alignment(void) {
    void* ptr1 = malloc(1);
    void* ptr2 = malloc(7);
    void* ptr3 = malloc(13);
    
    if (ptr1 == NULL || ptr2 == NULL || ptr3 == NULL) {
        TEST_FAILED("small allocation failed");
        tests_failed++;
        if (ptr1) free(ptr1);
        if (ptr2) free(ptr2);
        if (ptr3) free(ptr3);
        return;
    }
    
    if (((uintptr_t)ptr1 % 16) != 0 || 
        ((uintptr_t)ptr2 % 16) != 0 || 
        ((uintptr_t)ptr3 % 16) != 0) {
        TEST_FAILED("pointers not 16-byte aligned");
        tests_failed++;
        free(ptr1);
        free(ptr2);
        free(ptr3);
        return;
    }
    
    free(ptr1);
    free(ptr2);
    free(ptr3);
    TEST_PASSED();
    tests_passed++;
}

void test_fragmentation_pattern(void) {
    void* ptrs[10];
    
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(128);
        if (ptrs[i] == NULL) {
            TEST_FAILED("allocation failed");
            tests_failed++;
            for (int j = 0; j < i; j++) {
                free(ptrs[j]);
            }
            return;
        }
    }
    
    for (int i = 0; i < 10; i += 2) {
        free(ptrs[i]);
    }
    
    void* new_ptr = malloc(64);
    if (new_ptr == NULL) {
        TEST_FAILED("allocation after fragmentation failed");
        tests_failed++;
        for (int i = 1; i < 10; i += 2) {
            free(ptrs[i]);
        }
        return;
    }
    
    free(new_ptr);
    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_sequential_realloc(void) {
    void* ptr = malloc(32);
    if (ptr == NULL) {
        TEST_FAILED("initial allocation failed");
        tests_failed++;
        return;
    }
    
    size_t sizes[] = {64, 128, 256, 512, 1024, 512, 256, 128};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        void* new_ptr = realloc(ptr, sizes[i]);
        if (new_ptr == NULL) {
            TEST_FAILED("realloc failed in sequence");
            tests_failed++;
            free(ptr);
            return;
        }
        ptr = new_ptr;
    }
    
    free(ptr);
    TEST_PASSED();
    tests_passed++;
}

int main(void) {
    printf("Running Comprehensive Memory Allocator Tests\n");
    printf("=============================================\n\n");
    
    test_malloc_basic();
    test_malloc_zero_size();
    test_free_null();
    test_calloc_basic();
    test_calloc_zero_params();
    test_calloc_overflow();
    test_realloc_null_block();
    test_realloc_expand();
    test_realloc_shrink();
    test_multiple_allocations();
    test_allocation_reuse();
    test_large_allocation();
    test_alternating_alloc_free();
    test_mixed_sizes();
    test_struct_allocation();
    test_alignment();
    test_fragmentation_pattern();
    test_sequential_realloc();
    
    printf("\n=============================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("  Total:  %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n✓ All tests passed!\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed.\n");
        return 1;
    }
}
