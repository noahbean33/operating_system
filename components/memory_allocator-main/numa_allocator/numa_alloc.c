#define _GNU_SOURCE
#include "numa_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <numa.h>
#include <numaif.h>

/* Configuration Constants */
#define MAX_NUMA_NODES 8
#define SIZE_CLASSES 8
#define REFILL_BATCH_SIZE 64
#define HUGE_PAGE_SIZE (2 * 1024 * 1024)  // 2MB

/* Size classes for small allocations (16B to 2KB) */
static const size_t SIZE_CLASS_SIZES[SIZE_CLASSES] = {
    16, 32, 64, 128, 256, 512, 1024, 2048
};

/* ============================================================================
 * Data Structures
 * ========================================================================= */

/* NUMA Topology Information */
typedef struct {
    int num_nodes;
    int num_cpus;
    int *cpu_to_node_map;
} numa_topology_t;

/* Free list node for size-class allocations */
typedef struct free_list {
    struct free_list *next;
} free_list_t;

/* Thread-local arena (lock-free fast path) */
typedef struct {
    int numa_node;
    free_list_t *free_lists[SIZE_CLASSES];
    size_t stats_allocs;
    size_t stats_frees;
} thread_arena_t;

/* Per-NUMA-node memory pool */
typedef struct {
    int node_id;
    void *base_addr;
    size_t total_size;
    size_t used;
    pthread_mutex_t lock;
} node_pool_t;

/* Block header for tracking allocations */
typedef struct {
    size_t size;
    int size_class;  // -1 for large allocations
    int numa_node;
} block_header_t;

/* ============================================================================
 * Global State
 * ========================================================================= */

static numa_topology_t *g_topology = NULL;
static node_pool_t *g_node_pools[MAX_NUMA_NODES] = {NULL};
static int g_initialized = 0;

/* Thread-local storage - no locks needed! */
static __thread thread_arena_t *my_arena = NULL;

/* ============================================================================
 * NUMA Topology Discovery
 * ========================================================================= */

static numa_topology_t *discover_numa_topology(void) {
    numa_topology_t *topo = calloc(1, sizeof(*topo));
    if (!topo) return NULL;
    
    if (numa_available() == -1) {
        /* Fallback: single node system */
        fprintf(stderr, "[NUMA] libnuma not available, using single-node fallback\n");
        topo->num_nodes = 1;
        topo->num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
        topo->cpu_to_node_map = calloc(topo->num_cpus, sizeof(int));
        if (!topo->cpu_to_node_map) {
            free(topo);
            return NULL;
        }
        /* All CPUs map to node 0 */
        for (int i = 0; i < topo->num_cpus; i++) {
            topo->cpu_to_node_map[i] = 0;
        }
        return topo;
    }
    
    topo->num_nodes = numa_max_node() + 1;
    topo->num_cpus = numa_num_configured_cpus();
    topo->cpu_to_node_map = calloc(topo->num_cpus, sizeof(int));
    
    if (!topo->cpu_to_node_map) {
        free(topo);
        return NULL;
    }
    
    for (int cpu = 0; cpu < topo->num_cpus; cpu++) {
        topo->cpu_to_node_map[cpu] = numa_node_of_cpu(cpu);
    }
    
    return topo;
}

static int get_current_numa_node(void) {
    if (!g_topology) return 0;
    
    int cpu = sched_getcpu();
    if (cpu < 0 || cpu >= g_topology->num_cpus) {
        return 0;
    }
    
    return g_topology->cpu_to_node_map[cpu];
}

/* ============================================================================
 * Per-NUMA-Node Memory Pools
 * ========================================================================= */

static node_pool_t *create_node_pool(int node_id, size_t size) {
    node_pool_t *pool = calloc(1, sizeof(*pool));
    if (!pool) return NULL;
    
    pool->node_id = node_id;
    pool->total_size = size;
    pool->used = 0;
    
    /* Allocate memory on specific NUMA node */
    if (numa_available() != -1 && node_id < numa_max_node() + 1) {
        pool->base_addr = numa_alloc_onnode(size, node_id);
    } else {
        /* Fallback to regular allocation */
        pool->base_addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pool->base_addr == MAP_FAILED) {
            pool->base_addr = NULL;
        }
    }
    
    if (!pool->base_addr) {
        fprintf(stderr, "[NUMA] Failed to allocate pool on node %d\n", node_id);
        free(pool);
        return NULL;
    }
    
    /* Touch pages to ensure first-touch policy places them on this node */
    memset(pool->base_addr, 0, size);
    
    pthread_mutex_init(&pool->lock, NULL);
    
    return pool;
}

/* ============================================================================
 * Thread-Local Arenas
 * ========================================================================= */

static thread_arena_t *create_arena(void) {
    thread_arena_t *arena = calloc(1, sizeof(*arena));
    if (!arena) return NULL;
    
    arena->numa_node = get_current_numa_node();
    arena->stats_allocs = 0;
    arena->stats_frees = 0;
    
    /* Initialize empty free lists */
    for (int i = 0; i < SIZE_CLASSES; i++) {
        arena->free_lists[i] = NULL;
    }
    
    return arena;
}

static thread_arena_t *get_or_create_arena(void) {
    if (my_arena) {
        return my_arena;
    }
    
    my_arena = create_arena();
    return my_arena;
}

/* Map requested size to size class index */
static int get_size_class(size_t size) {
    for (int i = 0; i < SIZE_CLASSES; i++) {
        if (size <= SIZE_CLASS_SIZES[i]) {
            return i;
        }
    }
    return -1;  /* Too large for size classes */
}

/* ============================================================================
 * Large Block Allocation (with Huge Pages)
 * ========================================================================= */

static void *allocate_large_block(int numa_node, size_t size) {
    /* Add space for header */
    size_t total_size = size + sizeof(block_header_t);
    
    /* Round up to huge page size for large allocations */
    size_t aligned_size = total_size;
    if (total_size >= HUGE_PAGE_SIZE) {
        aligned_size = (total_size + HUGE_PAGE_SIZE - 1) & ~(HUGE_PAGE_SIZE - 1);
    }
    
    void *ptr = NULL;
    
    /* Try to allocate with huge pages */
    if (aligned_size >= HUGE_PAGE_SIZE) {
        ptr = mmap(NULL, aligned_size,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                   -1, 0);
        
        if (ptr == MAP_FAILED) {
            /* Fallback to regular pages */
            ptr = mmap(NULL, aligned_size,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS,
                       -1, 0);
        }
    } else {
        ptr = mmap(NULL, aligned_size,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1, 0);
    }
    
    if (ptr == MAP_FAILED) {
        return NULL;
    }
    
    /* Bind to NUMA node */
    if (numa_available() != -1 && numa_node < numa_max_node() + 1) {
        unsigned long nodemask = 1UL << numa_node;
        mbind(ptr, aligned_size, MPOL_BIND, &nodemask, sizeof(nodemask) * 8, 0);
    }
    
    /* Touch pages to ensure placement */
    memset(ptr, 0, aligned_size);
    
    /* Setup header */
    block_header_t *header = (block_header_t *)ptr;
    header->size = aligned_size;
    header->size_class = -1;
    header->numa_node = numa_node;
    
    return (void *)((char *)ptr + sizeof(block_header_t));
}

static void free_large_block(void *ptr) {
    if (!ptr) return;
    
    block_header_t *header = (block_header_t *)((char *)ptr - sizeof(block_header_t));
    void *base = (void *)header;
    size_t size = header->size;
    
    munmap(base, size);
}

/* ============================================================================
 * Slow Path - Batch Refill from Node Pool
 * ========================================================================= */

static void *slow_path_alloc(thread_arena_t *arena, size_t size, int size_class) {
    /* Large allocation - bypass size classes */
    if (size_class < 0) {
        return allocate_large_block(arena->numa_node, size);
    }
    
    /* Refill this size class from node pool */
    node_pool_t *pool = g_node_pools[arena->numa_node];
    if (!pool) {
        return NULL;
    }
    
    size_t block_size = SIZE_CLASS_SIZES[size_class];
    
    pthread_mutex_lock(&pool->lock);
    
    /* Allocate batch of blocks */
    size_t total_needed = (block_size + sizeof(block_header_t)) * REFILL_BATCH_SIZE;
    if (pool->used + total_needed > pool->total_size) {
        pthread_mutex_unlock(&pool->lock);
        fprintf(stderr, "[NUMA] Node %d pool exhausted\n", arena->numa_node);
        return NULL;
    }
    
    void *batch_start = (char *)pool->base_addr + pool->used;
    pool->used += total_needed;
    
    pthread_mutex_unlock(&pool->lock);
    
    /* Carve batch into individual blocks with headers */
    free_list_t *head = NULL;
    size_t stride = block_size + sizeof(block_header_t);
    
    for (int i = 0; i < REFILL_BATCH_SIZE; i++) {
        void *block_base = (char *)batch_start + i * stride;
        block_header_t *header = (block_header_t *)block_base;
        header->size = block_size;
        header->size_class = size_class;
        header->numa_node = arena->numa_node;
        
        free_list_t *block = (free_list_t *)((char *)block_base + sizeof(block_header_t));
        block->next = head;
        head = block;
    }
    
    /* Take one block, put rest in free list */
    void *result = (void *)head;
    arena->free_lists[size_class] = head->next;
    
    return result;
}

/* ============================================================================
 * Fast Path Allocation (Lock-Free)
 * ========================================================================= */

void *numalloc(size_t size) {
    if (size == 0) return NULL;
    
    if (!g_initialized) {
        fprintf(stderr, "[NUMA] Allocator not initialized! Call numalloc_init() first\n");
        return NULL;
    }
    
    thread_arena_t *arena = get_or_create_arena();
    if (!arena) return NULL;
    
    int size_class = get_size_class(size);
    
    /* FAST PATH: Small allocation with cached free block */
    if (size_class >= 0 && arena->free_lists[size_class]) {
        free_list_t *block = arena->free_lists[size_class];
        arena->free_lists[size_class] = block->next;
        arena->stats_allocs++;
        return (void *)block;
    }
    
    /* SLOW PATH: Need to get more memory */
    void *ptr = slow_path_alloc(arena, size, size_class);
    if (ptr) {
        arena->stats_allocs++;
    }
    return ptr;
}

/* ============================================================================
 * Free Operations
 * ========================================================================= */

void numalloc_free(void *ptr) {
    if (!ptr) return;
    
    /* Get block header */
    block_header_t *header = (block_header_t *)((char *)ptr - sizeof(block_header_t));
    int size_class = header->size_class;
    
    if (size_class < 0) {
        /* Large block - free directly */
        free_large_block(ptr);
        return;
    }
    
    /* Small block - return to thread-local free list */
    thread_arena_t *arena = get_or_create_arena();
    if (!arena) return;
    
    /* FAST PATH: Return to thread-local free list */
    free_list_t *block = (free_list_t *)ptr;
    block->next = arena->free_lists[size_class];
    arena->free_lists[size_class] = block;
    arena->stats_frees++;
}

/* ============================================================================
 * Public API Implementation
 * ========================================================================= */

int numalloc_init(size_t pool_size_per_node) {
    if (g_initialized) {
        fprintf(stderr, "[NUMA] Allocator already initialized\n");
        return -1;
    }
    
    /* Discover NUMA topology */
    g_topology = discover_numa_topology();
    if (!g_topology) {
        fprintf(stderr, "[NUMA] Failed to discover topology\n");
        return -1;
    }
    
    printf("[NUMA] Detected %d NUMA node(s), %d CPU(s)\n",
           g_topology->num_nodes, g_topology->num_cpus);
    
    /* Create memory pool for each NUMA node */
    for (int i = 0; i < g_topology->num_nodes; i++) {
        g_node_pools[i] = create_node_pool(i, pool_size_per_node);
        if (!g_node_pools[i]) {
            fprintf(stderr, "[NUMA] Failed to create pool for node %d\n", i);
            return -1;
        }
        printf("[NUMA] Created %zu MB pool on node %d\n",
               pool_size_per_node / (1024 * 1024), i);
    }
    
    g_initialized = 1;
    return 0;
}

void *numalloc_calloc(size_t num, size_t size) {
    if (num == 0 || size == 0) return NULL;
    
    size_t total = num * size;
    /* Check for overflow */
    if (size != total / num) return NULL;
    
    void *ptr = numalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *numalloc_realloc(void *ptr, size_t size) {
    if (!ptr) return numalloc(size);
    if (size == 0) {
        numalloc_free(ptr);
        return NULL;
    }
    
    /* Get old size from header */
    block_header_t *header = (block_header_t *)((char *)ptr - sizeof(block_header_t));
    size_t old_size = header->size;
    
    if (header->size_class >= 0) {
        old_size = SIZE_CLASS_SIZES[header->size_class];
    }
    
    /* If new size fits in current block, just return it */
    if (size <= old_size) {
        return ptr;
    }
    
    /* Allocate new block and copy */
    void *new_ptr = numalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, old_size);
        numalloc_free(ptr);
    }
    return new_ptr;
}

void numalloc_get_thread_stats(size_t *allocs, size_t *frees) {
    thread_arena_t *arena = my_arena;
    if (arena) {
        if (allocs) *allocs = arena->stats_allocs;
        if (frees) *frees = arena->stats_frees;
    } else {
        if (allocs) *allocs = 0;
        if (frees) *frees = 0;
    }
}

void numalloc_print_topology(void) {
    if (!g_topology) {
        printf("[NUMA] Topology not initialized\n");
        return;
    }
    
    printf("\n=== NUMA Topology ===\n");
    printf("Nodes: %d\n", g_topology->num_nodes);
    printf("CPUs: %d\n", g_topology->num_cpus);
    printf("\nCPU-to-Node Mapping:\n");
    
    for (int i = 0; i < g_topology->num_cpus; i++) {
        printf("  CPU %2d -> Node %d\n", i, g_topology->cpu_to_node_map[i]);
    }
    printf("====================\n\n");
}

void numalloc_cleanup(void) {
    if (!g_initialized) return;
    
    /* Cleanup node pools */
    for (int i = 0; i < g_topology->num_nodes; i++) {
        if (g_node_pools[i]) {
            if (g_node_pools[i]->base_addr) {
                if (numa_available() != -1) {
                    numa_free(g_node_pools[i]->base_addr, g_node_pools[i]->total_size);
                } else {
                    munmap(g_node_pools[i]->base_addr, g_node_pools[i]->total_size);
                }
            }
            pthread_mutex_destroy(&g_node_pools[i]->lock);
            free(g_node_pools[i]);
            g_node_pools[i] = NULL;
        }
    }
    
    /* Cleanup topology */
    if (g_topology) {
        if (g_topology->cpu_to_node_map) {
            free(g_topology->cpu_to_node_map);
        }
        free(g_topology);
        g_topology = NULL;
    }
    
    g_initialized = 0;
    printf("[NUMA] Allocator cleaned up\n");
}
