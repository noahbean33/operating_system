#ifndef NUMA_ALLOC_H
#define NUMA_ALLOC_H

#include <stddef.h>

/**
 * NUMA-Aware Memory Allocator for HPC Systems
 * 
 * Features:
 * - NUMA-aware allocation from local node
 * - Lock-free fast path using thread-local arenas
 * - Size-class segregation (16B to 2KB)
 * - Huge page support for large allocations
 * - Scalable to 100+ threads
 * 
 * Note: Function names use 'numalloc' prefix to avoid conflicts with libnuma
 */

/**
 * Initialize the NUMA allocator
 * Must be called before any allocations
 * 
 * @param pool_size_per_node: Size of memory pool for each NUMA node (e.g., 1GB)
 * @return 0 on success, -1 on failure
 */
int numalloc_init(size_t pool_size_per_node);

/**
 * Allocate memory from local NUMA node
 * 
 * @param size: Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void *numalloc(size_t size);

/**
 * Free memory allocated by numalloc
 * 
 * @param ptr: Pointer to memory to free
 */
void numalloc_free(void *ptr);

/**
 * Allocate and zero-initialize memory from local NUMA node
 * 
 * @param num: Number of elements
 * @param size: Size of each element
 * @return Pointer to allocated memory, or NULL on failure
 */
void *numalloc_calloc(size_t num, size_t size);

/**
 * Resize an existing allocation
 * 
 * @param ptr: Pointer to existing allocation
 * @param size: New size in bytes
 * @return Pointer to resized memory, or NULL on failure
 */
void *numalloc_realloc(void *ptr, size_t size);

/**
 * Get allocation statistics for current thread
 * 
 * @param allocs: Output pointer for allocation count
 * @param frees: Output pointer for free count
 */
void numalloc_get_thread_stats(size_t *allocs, size_t *frees);

/**
 * Print NUMA topology information
 */
void numalloc_print_topology(void);

/**
 * Cleanup and free all allocator resources
 */
void numalloc_cleanup(void);

#endif /* NUMA_ALLOC_H */
