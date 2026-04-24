# Exploration: Memory Allocation

## Introduction

To run a program, the OS creates a process and loads the program into memory. In this exploration, we will examine the **layout of a process's memory**, study **dynamic memory allocation**, and identify common pitfalls associated with accessing memory in C programs.

---

## Process Memory Layout

The memory of a process is organized into several segments. From the **lowest address** to the **highest address**, the segments are:

1. **Code (Text)**
2. **Data**
3. **Heap**
4. **Stack**

The Code and Data segments have **fixed sizes**. The Heap and Stack segments can **grow and shrink** during execution. Typically (particularly on x86 architecture), the heap grows toward higher addresses and the stack grows toward lower addresses.

### Text Segment

The text segment (also called the **code segment**) contains the object code for the program. Its size and contents don't change over the execution of the process.

### Data Segment

The data segment contains memory allocated for **global** and **static** variables. This memory is allocated at the start of the program.

### Stack Segment

The stack segment contains memory for **non-static** and **non-global** variables. Its contents are maintained in **LIFO** (last in, first out) order.

When a function is called, the stack grows:
1. The **arguments** of the function are placed on the stack
2. Followed by the **return address** of the calling function
3. Then the **local variables** of the function itself

This is referred to as the **stack frame** (or activation frame) of the function. When a function returns, its stack frame is removed and the stack shrinks.

### Heap Segment

The heap segment contains **dynamically allocated** memory. The C functions `malloc` and `calloc` are used to allocate memory on the heap, while `free` is used to deallocate it.

---

## Dynamic Memory Allocation

Many times a program needs to allocate memory dynamically at **run-time** rather than at compile-time. For example, creating an array whose size is determined by user input.

### The `malloc` and `calloc` Functions

```c
void* malloc(size_t size);
```

- Takes one argument: the size in bytes of the block to allocate
- Returns a **pointer** to the allocated memory on success, or `NULL` on failure
- Does **not** initialize the memory

```c
void* calloc(size_t numItems, size_t size);
```

- First argument: how many elements to allocate
- Second argument: size of each element
- Allocates `numItems * size` bytes
- **Initializes memory to 0**
- Returns a pointer on success, `NULL` on failure

### `sizeof` Operator

C provides the `sizeof` operator that returns the **size of its argument in bytes**. It is very commonly used with `malloc` or `calloc`:

```c
#include <stdio.h>
#include <stdlib.h>

void printArray(int count, int* ptr){
    for(int i = 0; i < count; i++){
        printf("%d ", *(ptr + i));
    }
    printf("\n");
}

int main(void) {
    int count;
    int* arrayPtr;
    printf("How many integers you want to enter: ");
    scanf("%d", &count);

    // Allocate memory for number of integers equal to count
    arrayPtr = malloc(sizeof(int) * count);
    if(arrayPtr == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    } else {
        for(int i = 0; i < count; i++){
            printf("Enter an integer: ");
            scanf("%d", (arrayPtr + i));
        }
        printArray(count, arrayPtr);
    }
    return 0;
}
```

### The `free` Function

Memory allocated on the **stack** is freed automatically when a function returns. In contrast, memory allocated on the **heap** is only freed by calling `free`:

```c
void free(void *ptr);
```

Takes one argument: a pointer to the start of the memory block (the value returned by `calloc` or `malloc`).

---

## Example: Linked List with Dynamic Allocation

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for customer information */
struct customer {
  char* name;
  char* state;
  struct customer* next;
};

/*
 * Create a customer structure using dynamic memory allocation
 */
struct customer* createCustomer(char* name, char* state){
  struct customer* currCustomer = malloc(sizeof(struct customer));

  currCustomer->name = calloc(strlen(name) + 1, sizeof(char));
  strcpy(currCustomer->name, name);

  currCustomer->state = calloc(strlen(state) + 1, sizeof(char));
  strcpy(currCustomer->state, state);

  currCustomer->next = NULL;
  return currCustomer;
}

/*
 * Print the linked list of customers
 */
void printCustomerList(struct customer* list){
  while(list != NULL){
    printf("%s %s\n", list->name, list->state);
    list = list->next;
  }
}

int main(void){
  char name[16];
  char state[16];

  struct customer* head = NULL;
  struct customer* tail = NULL;
  struct customer* newElem;

  printf("Create a linked list with info of 3 customers\n");
  for(int i = 0; i < 3; i++){
    printf("Enter the name of the customer (up to 15 chars): ");
    scanf("%s", name);
    printf("Enter the state they live in (up to 15 chars): ");
    scanf("%s", state);

    newElem = createCustomer(name, state);

    if(head == NULL){
      head = newElem;
      tail = newElem;
    } else {
      tail->next = newElem;
      tail = newElem;
    }
  }
  printCustomerList(head);
  return 0;
}
```

> **Note:** Members of a pointer to a structure are accessed using `->` rather than `.`

---

## Pitfalls

### Memory Leaks

A **memory leak** occurs when a program allocates memory on the heap but doesn't free it when it's no longer needed:

```c
#include <stdio.h>
#include <stdlib.h>

void memoryLeak(){
    // ptr goes out of scope when the function returns
    // Memory is allocated but never freed
    int* ptr = (int*) malloc(sizeof(int) * 10);
    return;
}

int main(void) {
    memoryLeak();
    return 0;
}
```

Memory leaks reduce the amount of heap available for allocation. In **long-running programs** (servers, daemons), this can be a severe problem that results in program termination.

### Buffer Overflow

A **buffer overflow** happens when a program writing data to a buffer overruns the boundary of the buffer:

```c
#include <stdio.h>

int main(void) {
  char state[3];
  char message[] = {"Hi!"};

  printf("Enter the 2-letter code for the state you live in: ");
  scanf("%s", state);
  printf("%s you live in %s\n", message, state);
}
```

If the user enters a very long string (e.g., `Thisnameisveryverylong`), `scanf` tries to store it into `state` (which is only 3 bytes on the stack), causing the array to overflow and the program to crash.

---

## Additional Resources

- Chapters 12 and 13 of *Modern C* discuss memory allocation in C.
- *Computer Systems: A Programmer's Perspective* (3rd ed.) by Bryant & O'Hallaron (Pearson, 2016) — detailed discussion of memory-related bugs.
