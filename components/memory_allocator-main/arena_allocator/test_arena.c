#include "arena_allocator.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_PASSED() printf("  [PASS] %s\n", __func__)
#define TEST_FAILED(msg) printf("  [FAIL] %s: %s\n", __func__, msg)

static int tests_passed = 0;
static int tests_failed = 0;

void test_arena_create_destroy(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    if (arena == NULL) {
        TEST_FAILED("arena_create returned NULL");
        tests_failed++;
        return;
    }
    
    if (arena->reserve_size == 0 || arena->commit_size == 0) {
        TEST_FAILED("invalid arena sizes");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_arena_create_invalid_params(void) {
    arena_allocator_t* arena1 = arena_create(0, ARENA_KILOBYTES(64));
    arena_allocator_t* arena2 = arena_create(ARENA_MEGABYTES(1), 0);
    
    if (arena1 != NULL || arena2 != NULL) {
        TEST_FAILED("should return NULL for invalid parameters");
        tests_failed++;
        if (arena1) arena_destroy(arena1);
        if (arena2) arena_destroy(arena2);
        return;
    }
    
    TEST_PASSED();
    tests_passed++;
}

void test_basic_allocation(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    void* ptr = arena_alloc(arena, 128);
    
    if (ptr == NULL) {
        TEST_FAILED("allocation failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    memset(ptr, 0xAB, 128);
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_multiple_allocations(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    void* ptr1 = arena_alloc(arena, 64);
    void* ptr2 = arena_alloc(arena, 128);
    void* ptr3 = arena_alloc(arena, 256);
    
    if (ptr1 == NULL || ptr2 == NULL || ptr3 == NULL) {
        TEST_FAILED("one or more allocations failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    if (ptr1 == ptr2 || ptr2 == ptr3 || ptr1 == ptr3) {
        TEST_FAILED("pointers overlap");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    memset(ptr1, 0x11, 64);
    memset(ptr2, 0x22, 128);
    memset(ptr3, 0x33, 256);
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_allocation_zero_initialized(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    size_t size = 1024;
    unsigned char* ptr = (unsigned char*)arena_alloc(arena, size);
    
    if (ptr == NULL) {
        TEST_FAILED("allocation failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    for (size_t i = 0; i < size; i++) {
        if (ptr[i] != 0) {
            TEST_FAILED("memory not zero-initialized");
            tests_failed++;
            arena_destroy(arena);
            return;
        }
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_alignment(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    void* ptr1 = arena_alloc_aligned(arena, 1, 16);
    void* ptr2 = arena_alloc_aligned(arena, 1, 32);
    void* ptr3 = arena_alloc_aligned(arena, 1, 64);
    
    if (ptr1 == NULL || ptr2 == NULL || ptr3 == NULL) {
        TEST_FAILED("aligned allocation failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    if (((uintptr_t)ptr1 % 16) != 0) {
        TEST_FAILED("16-byte alignment incorrect");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    if (((uintptr_t)ptr2 % 32) != 0) {
        TEST_FAILED("32-byte alignment incorrect");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    if (((uintptr_t)ptr3 % 64) != 0) {
        TEST_FAILED("64-byte alignment incorrect");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_arena_reset(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    arena_alloc(arena, 1024);
    arena_alloc(arena, 2048);
    
    size_t pos_before_reset = arena_get_position(arena);
    
    arena_reset(arena);
    
    size_t pos_after_reset = arena_get_position(arena);
    
    if (pos_after_reset >= pos_before_reset) {
        TEST_FAILED("position not reset");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    void* ptr = arena_alloc(arena, 512);
    if (ptr == NULL) {
        TEST_FAILED("allocation after reset failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_position_get_set(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    size_t initial_pos = arena_get_position(arena);
    
    arena_alloc(arena, 1024);
    size_t after_alloc_pos = arena_get_position(arena);
    
    if (after_alloc_pos <= initial_pos) {
        TEST_FAILED("position did not advance after allocation");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_set_position(arena, initial_pos);
    size_t restored_pos = arena_get_position(arena);
    
    if (restored_pos != initial_pos) {
        TEST_FAILED("position not restored correctly");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_large_allocation(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(128), ARENA_MEGABYTES(1));
    
    void* ptr = arena_alloc(arena, ARENA_MEGABYTES(16));
    
    if (ptr == NULL) {
        TEST_FAILED("large allocation failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    memset(ptr, 0xFF, ARENA_MEGABYTES(16));
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_allocation_exceeds_reserve(void) {
    arena_allocator_t* arena = arena_create(ARENA_KILOBYTES(64), ARENA_KILOBYTES(16));
    
    void* ptr = arena_alloc(arena, ARENA_MEGABYTES(1));
    
    if (ptr != NULL) {
        TEST_FAILED("should fail when exceeding reserve size");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_null_arena_handling(void) {
    void* ptr = arena_alloc(NULL, 128);
    
    if (ptr != NULL) {
        TEST_FAILED("should return NULL for NULL arena");
        tests_failed++;
        return;
    }
    
    arena_destroy(NULL);
    
    size_t pos = arena_get_position(NULL);
    if (pos != 0) {
        TEST_FAILED("get_position should return 0 for NULL arena");
        tests_failed++;
        return;
    }
    
    arena_set_position(NULL, 100);
    arena_reset(NULL);
    
    TEST_PASSED();
    tests_passed++;
}

void test_zero_size_allocation(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    void* ptr = arena_alloc(arena, 0);
    
    if (ptr != NULL) {
        TEST_FAILED("should return NULL for zero size allocation");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_many_small_allocations(void) {
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(16), ARENA_KILOBYTES(64));
    
    const int num_allocations = 10000;
    void* pointers[10000];
    
    for (int i = 0; i < num_allocations; i++) {
        pointers[i] = arena_alloc(arena, 16);
        if (pointers[i] == NULL) {
            TEST_FAILED("allocation failed during stress test");
            tests_failed++;
            arena_destroy(arena);
            return;
        }
        memset(pointers[i], (unsigned char)i, 16);
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

void test_struct_allocation(void) {
    typedef struct {
        int id;
        double value;
        char name[32];
    } TestStruct;
    
    arena_allocator_t* arena = arena_create(ARENA_MEGABYTES(1), ARENA_KILOBYTES(64));
    
    TestStruct* s1 = (TestStruct*)arena_alloc(arena, sizeof(TestStruct));
    TestStruct* s2 = (TestStruct*)arena_alloc(arena, sizeof(TestStruct));
    
    if (s1 == NULL || s2 == NULL) {
        TEST_FAILED("struct allocation failed");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    s1->id = 42;
    s1->value = 3.14159;
    strcpy(s1->name, "Test Structure");
    
    s2->id = 100;
    s2->value = 2.71828;
    strcpy(s2->name, "Another Test");
    
    if (s1->id != 42 || s2->id != 100) {
        TEST_FAILED("struct data corrupted");
        tests_failed++;
        arena_destroy(arena);
        return;
    }
    
    arena_destroy(arena);
    TEST_PASSED();
    tests_passed++;
}

int main(void) {
    printf("Running Arena Allocator Tests\n");
    printf("==============================\n\n");
    
    test_arena_create_destroy();
    test_arena_create_invalid_params();
    test_basic_allocation();
    test_multiple_allocations();
    test_allocation_zero_initialized();
    test_alignment();
    test_arena_reset();
    test_position_get_set();
    test_large_allocation();
    test_allocation_exceeds_reserve();
    test_null_arena_handling();
    test_zero_size_allocation();
    test_many_small_allocations();
    test_struct_allocation();
    
    printf("\n==============================\n");
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
