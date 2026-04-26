#include <stdio.h>
#include "memalloc.h"

int main() {
    printf("Testing custom memory allocator...\n\n");
    
    printf("Test 1: malloc\n");
    int *arr = (int*)malloc(5 * sizeof(int));
    if (arr) {
        printf("  Allocated array of 5 integers\n");
        for (int i = 0; i < 5; i++) {
            arr[i] = i * 10;
        }
        printf("  Array contents: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }
    
    printf("\nTest 2: calloc\n");
    int *zeros = (int*)calloc(5, sizeof(int));
    if (zeros) {
        printf("  Allocated and zeroed array of 5 integers\n");
        printf("  Array contents: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", zeros[i]);
        }
        printf("\n");
    }
    
    printf("\nTest 3: realloc\n");
    arr = (int*)realloc(arr, 10 * sizeof(int));
    if (arr) {
        printf("  Reallocated array to 10 integers\n");
        for (int i = 5; i < 10; i++) {
            arr[i] = i * 10;
        }
        printf("  Array contents: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }
    
    printf("\nTest 4: free\n");
    free(arr);
    free(zeros);
    printf("  Freed all allocated memory\n");
    
    printf("\nAll tests completed successfully!\n");
    
    return 0;
}
