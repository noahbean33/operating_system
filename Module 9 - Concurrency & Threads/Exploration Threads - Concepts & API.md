# Exploration: Threads — Concepts & API

## Basic Concepts

**Threads** provide a mechanism for a process to do multiple things concurrently. Each thread has its own:
- Stack
- Registers
- Program counter

All threads in a process **share**:
- Code, data, and heap segments (the address space)

### Consequences

- Thread creation is **less expensive** than process creation
- If one thread blocks, other threads can still run
- Context switching between threads is **much faster** than between processes
- Communication between threads is simpler (shared memory)

> Threads are sometimes called **lightweight processes**.

### Kernel-Level vs. User-Level Threads

- **Kernel-level threads** — managed by the OS scheduler (Linux)
- **User-level threads (green threads)** — managed by a library; OS only sees processes

---

## Pthreads API

Standard POSIX API for threads on Unix. Header: `<pthread.h>`.

Compile with:
```bash
gcc -std=gnu99 -pthread -o main main.c
```

---

## Creating a Thread

```c
#include <pthread.h>

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);
```

| Parameter | Description |
|-----------|-------------|
| `thread` | Pointer to store the new thread's ID |
| `attr` | Thread attributes (or `NULL` for defaults) |
| `start_routine` | Function the thread will execute |
| `arg` | Argument passed to `start_routine` (or `NULL`) |

### Hello World Example

```c
#include <pthread.h>
#include <stdio.h>

void *helloWorld(void *args){
  printf("Hello World!\n");
  return NULL;
}

int main(void) {
  pthread_t tid;
  pthread_create(&tid, NULL, helloWorld, NULL);
  pthread_join(tid, NULL);
  return 0;
}
```

### Passing Multiple Arguments via a Struct

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct item{
  int year;
  char* name;
};

void* perform_work(void* argument){
  struct item* curr_item = (struct item*) argument;
  printf("name = %s, year = %d\n", curr_item->name, curr_item->year);
  return NULL;
}

int main(void){
  struct item* an_item = calloc(1, sizeof(struct item));
  an_item->name = strdup("John Doe");
  an_item->year = 2012;

  pthread_t t;
  pthread_create(&t, NULL, perform_work, (void *) an_item);
  pthread_join(t, NULL);

  free(an_item->name);
  free(an_item);
  exit(EXIT_SUCCESS);
}
```

---

## Ending a Thread

A thread can end by:

1. Calling `pthread_exit(retval)` — terminates the calling thread
2. Returning from `start_routine` (equivalent to `pthread_exit()`)
3. Being cancelled by another thread via `pthread_cancel(thread)`
4. Any thread calling `exit()` or `main()` returning — terminates the **entire process**

---

## Waiting for a Thread

```c
int pthread_join(pthread_t thread, void **retval);
```

Blocks until the specified thread terminates. Unlike processes, **any thread** can join any other thread (no parent-child hierarchy).

### Example: Multiple Threads

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NUM_THREADS 5

void* perform_work(void* argument){
  int passed_in_value = *((int *) argument);
  printf("It's me, thread with argument %d!\n", passed_in_value);
  return NULL;
}

int main(void){
  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    thread_args[i] = i;
    printf("In main: creating thread %d\n", i);
    int rc = pthread_create(&threads[i], NULL, perform_work, (void *) &thread_args[i]);
    assert(0 == rc);
  }

  for (int i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
    printf("In main: thread %d has completed\n", i);
  }
  printf("In main: All threads completed successfully\n");
  exit(EXIT_SUCCESS);
}
```

---

## Getting and Comparing Thread IDs

```c
pthread_t pthread_self(void);                    // Get current thread's ID
int pthread_equal(pthread_t t1, pthread_t t2);   // Compare two thread IDs
```

- `pthread_t` is platform-dependent — do not compare with `==`; use `pthread_equal()`
- `pthread_equal()` returns nonzero if equal, 0 otherwise

---

## Exercise

```c
int main(void){
  pthread_t t1, t2;
  pthread_create(&t1, NULL, perform_work, NULL);
  pthread_join(t1, NULL);       // Waits for t1 before creating t2
  pthread_create(&t2, NULL, perform_work, NULL);
  pthread_join(t2, NULL);
  // t1 and t2 do NOT run concurrently!
}
```

**Fix:** Create both threads first, then join both.

---

## Additional Resources

- Chapters 29–32 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
- [Chapter 26: Concurrency — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
