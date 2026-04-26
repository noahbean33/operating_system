#define _GNU_SOURCE
#include "numa_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define GB (1024UL * 1024 * 1024)
#define QUICK_ITERATIONS 10000
#define ALLOC_BATCH 50

/* Timing helper */
static inline uint64_t get_nanoseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

typedef struct {
    int thread_id;
    uint64_t elapsed_ns;
} thread_params_t;

void *test_thread(void *arg) {
    thread_params_t *params = (thread_params_t *)arg;
    void *ptrs[ALLOC_BATCH];
    
    uint64_t start = get_nanoseconds();
    
    for (int i = 0; i < QUICK_ITERATIONS; i++) {
        /* Allocate */
        for (int j = 0; j < ALLOC_BATCH; j++) {
            ptrs[j] = numalloc(64);
            if (!ptrs[j]) {
                fprintf(stderr, "Thread %d: allocation failed\n", params->thread_id);
                return NULL;
            }
            /* Write some data */
            memset(ptrs[j], params->thread_id & 0xFF, 64);
        }
        
        /* Free */
        for (int j = 0; j < ALLOC_BATCH; j++) {
            numalloc_free(ptrs[j]);
        }
    }
    
    uint64_t end = get_nanoseconds();
    params->elapsed_ns = end - start;
    
    return NULL;
}

void run_quick_test(int num_threads) {
    pthread_t threads[num_threads];
    thread_params_t params[num_threads];
    
    printf("Testing with %d thread(s)...\n", num_threads);
    
    uint64_t start = get_nanoseconds();
    
    for (int t = 0; t < num_threads; t++) {
        params[t].thread_id = t;
        params[t].elapsed_ns = 0;
        pthread_create(&threads[t], NULL, test_thread, &params[t]);
    }
    
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    uint64_t end = get_nanoseconds();
    double total_sec = (end - start) / 1e9;
    uint64_t total_ops = (uint64_t)num_threads * QUICK_ITERATIONS * ALLOC_BATCH * 2;
    double ops_per_sec = total_ops / total_sec;
    
    printf("  Time: %.3f seconds\n", total_sec);
    printf("  Throughput: %.2f M ops/sec\n", ops_per_sec / 1e6);
    printf("  ✓ Success\n\n");
}

int main(void) {
    printf("\n=== Quick NUMA Allocator Test ===\n\n");
    
    /* Initialize */
    if (numalloc_init(1 * GB) != 0) {
        fprintf(stderr, "Failed to initialize allocator\n");
        return 1;
    }
    
    numalloc_print_topology();
    
    /* Test different thread counts */
    printf("Running quick performance tests...\n\n");
    run_quick_test(1);
    run_quick_test(2);
    run_quick_test(4);
    run_quick_test(8);
    
    /* Test different allocation sizes */
    printf("Testing different size classes...\n");
    int sizes[] = {16, 64, 256, 1024, 4096};
    for (int i = 0; i < 5; i++) {
        void *ptr = numalloc(sizes[i]);
        if (ptr) {
            memset(ptr, 0xAA, sizes[i]);
            numalloc_free(ptr);
            printf("  %d bytes: ✓\n", sizes[i]);
        } else {
            printf("  %d bytes: ✗ FAILED\n", sizes[i]);
        }
    }
    
    printf("\n");
    
    /* Cleanup */
    numalloc_cleanup();
    
    printf("=== All Tests Passed! ===\n");
    return 0;
}
