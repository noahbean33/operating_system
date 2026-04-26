#define _GNU_SOURCE
#include "numa_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <numa.h>
#include <numaif.h>

#define GB (1024UL * 1024 * 1024)

/* ============================================================================
 * NUMA Placement Verification
 * ========================================================================= */

void verify_numa_placement(void) {
    printf("=== NUMA Placement Verification ===\n\n");
    
    if (numa_available() == -1) {
        printf("NUMA not available - skipping placement verification\n");
        return;
    }
    
    int current_node = numa_node_of_cpu(sched_getcpu());
    printf("Current CPU is on NUMA node %d\n\n", current_node);
    
    /* Test small allocation */
    printf("Test 1: Small allocation (64 bytes)\n");
    void *ptr1 = numalloc(64);
    if (ptr1) {
        int status;
        void *pages = ptr1;
        int node;
        
        if (move_pages(0, 1, &pages, NULL, &node, 0) == 0) {
            printf("  Allocated on node %d (expected %d) - %s\n",
                   node, current_node,
                   node == current_node ? "✓ PASS" : "✗ FAIL");
        } else {
            printf("  Could not verify placement (move_pages failed)\n");
        }
        numalloc_free(ptr1);
    } else {
        printf("  Allocation failed\n");
    }
    
    /* Test medium allocation */
    printf("\nTest 2: Medium allocation (4KB)\n");
    void *ptr2 = numalloc(4096);
    if (ptr2) {
        int status;
        void *pages = ptr2;
        int node;
        
        if (move_pages(0, 1, &pages, NULL, &node, 0) == 0) {
            printf("  Allocated on node %d (expected %d) - %s\n",
                   node, current_node,
                   node == current_node ? "✓ PASS" : "✗ FAIL");
        } else {
            printf("  Could not verify placement (move_pages failed)\n");
        }
        numalloc_free(ptr2);
    } else {
        printf("  Allocation failed\n");
    }
    
    /* Test large allocation (should use huge pages if available) */
    printf("\nTest 3: Large allocation (4MB)\n");
    void *ptr3 = numalloc(4 * 1024 * 1024);
    if (ptr3) {
        int status;
        void *pages = ptr3;
        int node;
        
        if (move_pages(0, 1, &pages, NULL, &node, 0) == 0) {
            printf("  Allocated on node %d (expected %d) - %s\n",
                   node, current_node,
                   node == current_node ? "✓ PASS" : "✗ FAIL");
            printf("  (Large allocation may use huge pages)\n");
        } else {
            printf("  Could not verify placement (move_pages failed)\n");
        }
        numalloc_free(ptr3);
    } else {
        printf("  Allocation failed\n");
    }
    
    printf("\n");
}

/* ============================================================================
 * Functional Tests
 * ========================================================================= */

void test_basic_operations(void) {
    printf("=== Basic Operations Test ===\n\n");
    
    int passed = 0;
    int total = 0;
    
    /* Test 1: malloc and free */
    total++;
    printf("Test 1: malloc and free\n");
    void *ptr = numalloc(100);
    if (ptr) {
        memset(ptr, 0xAA, 100);
        numalloc_free(ptr);
        printf("  ✓ PASS\n\n");
        passed++;
    } else {
        printf("  ✗ FAIL: malloc returned NULL\n\n");
    }
    
    /* Test 2: calloc */
    total++;
    printf("Test 2: calloc\n");
    int *arr = (int *)numalloc_calloc(10, sizeof(int));
    if (arr) {
        int all_zero = 1;
        for (int i = 0; i < 10; i++) {
            if (arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        if (all_zero) {
            printf("  ✓ PASS: Memory zeroed\n\n");
            passed++;
        } else {
            printf("  ✗ FAIL: Memory not zeroed\n\n");
        }
        numalloc_free(arr);
    } else {
        printf("  ✗ FAIL: calloc returned NULL\n\n");
    }
    
    /* Test 3: realloc */
    total++;
    printf("Test 3: realloc\n");
    int *data = (int *)numalloc(5 * sizeof(int));
    if (data) {
        for (int i = 0; i < 5; i++) {
            data[i] = i * 10;
        }
        
        int *expanded = (int *)numalloc_realloc(data, 10 * sizeof(int));
        if (expanded) {
            int preserved = 1;
            for (int i = 0; i < 5; i++) {
                if (expanded[i] != i * 10) {
                    preserved = 0;
                    break;
                }
            }
            if (preserved) {
                printf("  ✓ PASS: Data preserved after realloc\n\n");
                passed++;
            } else {
                printf("  ✗ FAIL: Data corrupted\n\n");
            }
            numalloc_free(expanded);
        } else {
            printf("  ✗ FAIL: realloc returned NULL\n\n");
            numalloc_free(data);
        }
    } else {
        printf("  ✗ FAIL: initial malloc returned NULL\n\n");
    }
    
    /* Test 4: Multiple allocations */
    total++;
    printf("Test 4: Multiple allocations\n");
    void *ptrs[100];
    int success = 1;
    for (int i = 0; i < 100; i++) {
        ptrs[i] = numalloc(64);
        if (!ptrs[i]) {
            success = 0;
            break;
        }
    }
    if (success) {
        for (int i = 0; i < 100; i++) {
            numalloc_free(ptrs[i]);
        }
        printf("  ✓ PASS: 100 allocations succeeded\n\n");
        passed++;
    } else {
        printf("  ✗ FAIL: Some allocations failed\n\n");
        for (int i = 0; i < 100; i++) {
            if (ptrs[i]) numalloc_free(ptrs[i]);
        }
    }
    
    /* Test 5: Different size classes */
    total++;
    printf("Test 5: Size class allocations\n");
    int sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048};
    success = 1;
    for (int i = 0; i < 8; i++) {
        void *p = numalloc(sizes[i]);
        if (!p) {
            success = 0;
            printf("  Failed at size %d\n", sizes[i]);
        } else {
            numalloc_free(p);
        }
    }
    if (success) {
        printf("  ✓ PASS: All size classes work\n\n");
        passed++;
    } else {
        printf("  ✗ FAIL\n\n");
    }
    
    printf("Summary: %d/%d tests passed\n\n", passed, total);
}

/* ============================================================================
 * Thread Safety Test
 * ========================================================================= */

typedef struct {
    int thread_id;
    int iterations;
    int *error_count;
} thread_test_params_t;

void *thread_test_func(void *arg) {
    thread_test_params_t *params = (thread_test_params_t *)arg;
    
    for (int i = 0; i < params->iterations; i++) {
        void *ptrs[10];
        
        /* Allocate */
        for (int j = 0; j < 10; j++) {
            ptrs[j] = numalloc(64);
            if (!ptrs[j]) {
                __sync_fetch_and_add(params->error_count, 1);
                return NULL;
            }
            /* Write pattern */
            memset(ptrs[j], params->thread_id & 0xFF, 64);
        }
        
        /* Verify pattern */
        for (int j = 0; j < 10; j++) {
            unsigned char *data = (unsigned char *)ptrs[j];
            for (int k = 0; k < 64; k++) {
                if (data[k] != (params->thread_id & 0xFF)) {
                    __sync_fetch_and_add(params->error_count, 1);
                }
            }
        }
        
        /* Free */
        for (int j = 0; j < 10; j++) {
            numalloc_free(ptrs[j]);
        }
    }
    
    return NULL;
}

void test_thread_safety(void) {
    printf("=== Thread Safety Test ===\n\n");
    
    int num_threads = 16;
    int iterations = 10000;
    pthread_t threads[num_threads];
    thread_test_params_t params[num_threads];
    int error_count = 0;
    
    printf("Running with %d threads, %d iterations each\n", num_threads, iterations);
    
    for (int t = 0; t < num_threads; t++) {
        params[t].thread_id = t;
        params[t].iterations = iterations;
        params[t].error_count = &error_count;
        pthread_create(&threads[t], NULL, thread_test_func, &params[t]);
    }
    
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    if (error_count == 0) {
        printf("✓ PASS: No errors detected\n\n");
    } else {
        printf("✗ FAIL: %d errors detected\n\n", error_count);
    }
}

/* ============================================================================
 * Main
 * ========================================================================= */

int main(int argc, char *argv[]) {
    printf("\n=== NUMA Allocator Validation Tests ===\n\n");
    
    /* Initialize allocator */
    if (numalloc_init(1 * GB) != 0) {
        fprintf(stderr, "Failed to initialize NUMA allocator\n");
        return 1;
    }
    
    numalloc_print_topology();
    
    /* Run tests */
    test_basic_operations();
    verify_numa_placement();
    test_thread_safety();
    
    /* Print statistics */
    size_t allocs, frees;
    numalloc_get_thread_stats(&allocs, &frees);
    printf("Main thread stats: %zu allocs, %zu frees\n\n", allocs, frees);
    
    /* Cleanup */
    numalloc_cleanup();
    
    printf("=== Validation Complete ===\n");
    return 0;
}
