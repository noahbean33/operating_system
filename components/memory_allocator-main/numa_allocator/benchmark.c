#define _GNU_SOURCE
#include "numa_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

#define GB (1024UL * 1024 * 1024)
#define ITERATIONS 50000      // Reduced from 1M for laptop testing
#define ALLOC_BATCH 100

/* Timing helpers */
static inline uint64_t get_nanoseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/* Benchmark parameters */
typedef struct {
    int thread_id;
    int iterations;
    int alloc_size;
    uint64_t elapsed_ns;
} benchmark_params_t;

/* ============================================================================
 * NUMA Allocator Benchmark
 * ========================================================================= */

void *numa_benchmark_thread(void *arg) {
    benchmark_params_t *params = (benchmark_params_t *)arg;
    void *ptrs[ALLOC_BATCH];
    
    uint64_t start = get_nanoseconds();
    
    for (int i = 0; i < params->iterations; i++) {
        /* Allocate batch */
        for (int j = 0; j < ALLOC_BATCH; j++) {
            ptrs[j] = numalloc(params->alloc_size);
            if (!ptrs[j]) {
                fprintf(stderr, "Thread %d: allocation failed at iteration %d\n",
                        params->thread_id, i);
                return NULL;
            }
        }
        
        /* Free batch */
        for (int j = 0; j < ALLOC_BATCH; j++) {
            numalloc_free(ptrs[j]);
        }
    }
    
    uint64_t end = get_nanoseconds();
    params->elapsed_ns = end - start;
    
    return NULL;
}

void run_numa_benchmark(int num_threads, int alloc_size) {
    pthread_t threads[num_threads];
    benchmark_params_t params[num_threads];
    
    printf("Running NUMA allocator benchmark: %d threads, %d byte allocations\n",
           num_threads, alloc_size);
    
    /* Create threads */
    for (int t = 0; t < num_threads; t++) {
        params[t].thread_id = t;
        params[t].iterations = ITERATIONS;
        params[t].alloc_size = alloc_size;
        params[t].elapsed_ns = 0;
        
        pthread_create(&threads[t], NULL, numa_benchmark_thread, &params[t]);
    }
    
    /* Wait for completion */
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    /* Calculate statistics */
    uint64_t total_ops = (uint64_t)num_threads * ITERATIONS * ALLOC_BATCH * 2; /* alloc + free */
    uint64_t max_time = 0;
    
    for (int t = 0; t < num_threads; t++) {
        if (params[t].elapsed_ns > max_time) {
            max_time = params[t].elapsed_ns;
        }
    }
    
    double ns_per_op = (double)max_time / (ITERATIONS * ALLOC_BATCH * 2);
    double ops_per_sec = 1000000000.0 / ns_per_op;
    
    printf("  Time: %.2f seconds\n", max_time / 1e9);
    printf("  Throughput: %.2f M ops/sec\n", ops_per_sec / 1e6);
    printf("  Latency: %.2f ns per operation\n", ns_per_op);
    
    /* Print per-thread stats */
    for (int t = 0; t < num_threads; t++) {
        size_t allocs, frees;
        /* Note: stats are per-thread, need to run in that thread */
    }
    
    printf("\n");
}

/* ============================================================================
 * Standard malloc Benchmark (for comparison)
 * ========================================================================= */

void *stdlib_benchmark_thread(void *arg) {
    benchmark_params_t *params = (benchmark_params_t *)arg;
    void *ptrs[ALLOC_BATCH];
    
    uint64_t start = get_nanoseconds();
    
    for (int i = 0; i < params->iterations; i++) {
        /* Allocate batch */
        for (int j = 0; j < ALLOC_BATCH; j++) {
            ptrs[j] = malloc(params->alloc_size);
            if (!ptrs[j]) {
                fprintf(stderr, "Thread %d: malloc failed at iteration %d\n",
                        params->thread_id, i);
                return NULL;
            }
        }
        
        /* Free batch */
        for (int j = 0; j < ALLOC_BATCH; j++) {
            free(ptrs[j]);
        }
    }
    
    uint64_t end = get_nanoseconds();
    params->elapsed_ns = end - start;
    
    return NULL;
}

void run_stdlib_benchmark(int num_threads, int alloc_size) {
    pthread_t threads[num_threads];
    benchmark_params_t params[num_threads];
    
    printf("Running standard malloc benchmark: %d threads, %d byte allocations\n",
           num_threads, alloc_size);
    
    /* Create threads */
    for (int t = 0; t < num_threads; t++) {
        params[t].thread_id = t;
        params[t].iterations = ITERATIONS;
        params[t].alloc_size = alloc_size;
        params[t].elapsed_ns = 0;
        
        pthread_create(&threads[t], NULL, stdlib_benchmark_thread, &params[t]);
    }
    
    /* Wait for completion */
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    /* Calculate statistics */
    uint64_t max_time = 0;
    
    for (int t = 0; t < num_threads; t++) {
        if (params[t].elapsed_ns > max_time) {
            max_time = params[t].elapsed_ns;
        }
    }
    
    double ns_per_op = (double)max_time / (ITERATIONS * ALLOC_BATCH * 2);
    double ops_per_sec = 1000000000.0 / ns_per_op;
    
    printf("  Time: %.2f seconds\n", max_time / 1e9);
    printf("  Throughput: %.2f M ops/sec\n", ops_per_sec / 1e6);
    printf("  Latency: %.2f ns per operation\n", ns_per_op);
    printf("\n");
}

/* ============================================================================
 * Scalability Test
 * ========================================================================= */

void run_scalability_test(void) {
    int thread_counts[] = {1, 2, 4, 8, 16, 32};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("\n=== Scalability Test (64-byte allocations) ===\n\n");
    printf("%-10s %-15s %-15s %-10s\n", "Threads", "NUMA (ns/op)", "stdlib (ns/op)", "Speedup");
    printf("%-10s %-15s %-15s %-10s\n", "-------", "------------", "--------------", "-------");
    
    for (int i = 0; i < num_tests; i++) {
        int num_threads = thread_counts[i];
        
        /* Run NUMA allocator benchmark */
        pthread_t threads[num_threads];
        benchmark_params_t numa_params[num_threads];
        
        for (int t = 0; t < num_threads; t++) {
            numa_params[t].thread_id = t;
            numa_params[t].iterations = ITERATIONS;
            numa_params[t].alloc_size = 64;
            numa_params[t].elapsed_ns = 0;
            pthread_create(&threads[t], NULL, numa_benchmark_thread, &numa_params[t]);
        }
        for (int t = 0; t < num_threads; t++) {
            pthread_join(threads[t], NULL);
        }
        
        uint64_t numa_max_time = 0;
        for (int t = 0; t < num_threads; t++) {
            if (numa_params[t].elapsed_ns > numa_max_time) {
                numa_max_time = numa_params[t].elapsed_ns;
            }
        }
        double numa_ns_per_op = (double)numa_max_time / (ITERATIONS * ALLOC_BATCH * 2);
        
        /* Run stdlib benchmark */
        benchmark_params_t stdlib_params[num_threads];
        
        for (int t = 0; t < num_threads; t++) {
            stdlib_params[t].thread_id = t;
            stdlib_params[t].iterations = ITERATIONS;
            stdlib_params[t].alloc_size = 64;
            stdlib_params[t].elapsed_ns = 0;
            pthread_create(&threads[t], NULL, stdlib_benchmark_thread, &stdlib_params[t]);
        }
        for (int t = 0; t < num_threads; t++) {
            pthread_join(threads[t], NULL);
        }
        
        uint64_t stdlib_max_time = 0;
        for (int t = 0; t < num_threads; t++) {
            if (stdlib_params[t].elapsed_ns > stdlib_max_time) {
                stdlib_max_time = stdlib_params[t].elapsed_ns;
            }
        }
        double stdlib_ns_per_op = (double)stdlib_max_time / (ITERATIONS * ALLOC_BATCH * 2);
        
        double speedup = stdlib_ns_per_op / numa_ns_per_op;
        
        printf("%-10d %-15.2f %-15.2f %-10.2fx\n",
               num_threads, numa_ns_per_op, stdlib_ns_per_op, speedup);
    }
    
    printf("\n");
}

/* ============================================================================
 * Main
 * ========================================================================= */

int main(int argc, char *argv[]) {
    printf("=== NUMA-Aware Memory Allocator Benchmark ===\n\n");
    
    /* Initialize NUMA allocator with 1GB per node */
    if (numalloc_init(1 * GB) != 0) {
        fprintf(stderr, "Failed to initialize NUMA allocator\n");
        return 1;
    }
    
    numalloc_print_topology();
    
    /* Run scalability test */
    run_scalability_test();
    
    /* Test different allocation sizes */
    printf("=== Size Class Performance (8 threads) ===\n\n");
    int sizes[] = {16, 64, 256, 1024, 4096};
    
    for (int i = 0; i < 5; i++) {
        run_numa_benchmark(8, sizes[i]);
    }
    
    /* Cleanup */
    numalloc_cleanup();
    
    printf("=== Benchmark Complete ===\n");
    return 0;
}
