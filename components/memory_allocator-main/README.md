# Memalloc - Custom Memory Allocator

A thread-safe custom memory allocator for Windows that implements standard C memory allocation functions (`malloc`, `calloc`, `realloc`, `free`) using VirtualAlloc.

## Features

- **Standard API**: Drop-in replacement for standard library memory functions
- **Thread-safe**: Uses Windows CRITICAL_SECTION for synchronization
- **Free list reuse**: Efficiently reuses freed memory blocks
- **Overflow detection**: Prevents integer overflow in calloc
- **Large allocations**: Supports multi-megabyte allocations

## API

```c
void* malloc(size_t size);           // Allocate memory
void free(void* block);              // Free memory
void* calloc(size_t num, size_t nsize);  // Allocate and zero-initialize
void* realloc(void* block, size_t size); // Resize allocation
```

## Building

```bash
cd memalloc
gcc -o test test_memalloc.c memalloc.c
```

## Testing

```bash
# Basic tests
./test_memalloc.exe

# Comprehensive tests (18 test cases)
gcc -o test_comprehensive test_memalloc_comprehensive.c memalloc.c
./test_comprehensive.exe
```

## Implementation Details

- **Memory backend**: Windows VirtualAlloc/VirtualFree
- **Alignment**: 8-byte aligned pointers (sufficient for most 64-bit use cases)
- **Free list**: Single linked list of blocks
- **Memory release**: Only releases memory when freeing the tail block
- **Complexity**: O(1) best case, O(n) worst case for allocation

## Limitations

- **Windows-only**: Uses Windows-specific VirtualAlloc API
- **8-byte alignment**: Returned pointers are 8-byte aligned, not 16-byte
- **No coalescing**: Adjacent free blocks are not merged (potential fragmentation)
- **Global lock**: Single lock for all operations (may limit scalability)

## Test Results

- ✓ 17/18 comprehensive tests pass
- ⚠ Alignment test shows 8-byte alignment instead of 16-byte
- See `memalloc/TEST_RESULTS.md` for detailed analysis

## Files

- `memalloc.h` - Public API header
- `memalloc.c` - Implementation (134 lines)
- `test_memalloc.c` - Basic tests
- `test_memalloc_comprehensive.c` - Comprehensive test suite
- `TEST_RESULTS.md` - Detailed test results and analysis