# Exploration: Synchronization for Concurrent Execution

## Race Conditions Revisited

Race conditions can occur within a single process when **multiple threads** execute operations on a shared resource unsafely. Because threads share memory, the risk is even **greater** than with multiple single-threaded processes.

### Example: Counter with Race Condition

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_TO 10000000
#define NUM_THREADS 3

int counter = 0;

void* perform_work(void* argument){
  for(int i = 0; i < COUNT_TO; i++){
    // THE CRITICAL SECTION
    counter += 1;
  }
  return NULL;
}

int main(void){
  pthread_t threads[NUM_THREADS];
  for(int i = 0; i < NUM_THREADS; i++)
    pthread_create(&threads[i], NULL, perform_work, NULL);
  for(int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  printf("Expected value of counter = %d\n", COUNT_TO * NUM_THREADS);
  printf("Actual value of counter = %d\n", counter);
  exit(EXIT_SUCCESS);
}
```

**Expected:** 30,000,000. **Actual:** varies on each run.

### Why?

`counter += 1` compiles to multiple machine instructions:

1. **Load** `counter` into a register
2. **Increment** the register
3. **Store** the register back to `counter`

The OS scheduler can interleave these instructions between threads, causing lost updates.

---

## Synchronizing Access via Mutex Locks

**Mutex** (mutual exclusion) — a lock that only one thread can hold at a time.

### Initializing and Destroying

```c
// Static initialization
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

// Dynamic initialization
pthread_mutex_init(&myMutex, NULL);

// Destruction (required for dynamically allocated mutexes)
pthread_mutex_destroy(&myMutex);
```

### Locking and Unlocking

```c
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex);    // Blocking lock
int pthread_mutex_trylock(pthread_mutex_t *mutex);  // Non-blocking (returns 0 on success)
int pthread_mutex_unlock(pthread_mutex_t *mutex);   // Unlock
```

- `pthread_mutex_lock()` blocks if the mutex is already locked
- `pthread_mutex_trylock()` returns non-zero if the mutex is already locked

### Example: Accurate Counter

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_TO 10000000
#define NUM_THREADS 3

pthread_mutex_t counterMutex;
int counter = 0;

void* perform_work(void* argument){
  for(int i = 0; i < COUNT_TO; i++){
    pthread_mutex_lock(&counterMutex);
    // CRITICAL SECTION
    counter += 1;
    pthread_mutex_unlock(&counterMutex);
  }
  return NULL;
}

int main(void){
  pthread_mutex_init(&counterMutex, NULL);
  pthread_t threads[NUM_THREADS];

  for(int i = 0; i < NUM_THREADS; i++)
    pthread_create(&threads[i], NULL, perform_work, NULL);
  for(int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  pthread_mutex_destroy(&counterMutex);
  printf("Expected value of counter = %d\n", COUNT_TO * NUM_THREADS);
  printf("Actual value of counter = %d\n", counter);
  exit(EXIT_SUCCESS);
}
```

> **Important:** Lock the mutex for the **shortest time necessary**. Statements unrelated to the shared resource should be outside the critical section to maximize concurrency.

---

## Additional Resources

- Chapters 29–32 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
- [Chapter 27: Interlude: Thread API — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
