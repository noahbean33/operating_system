# Exploration: Semaphores

## Introduction

POSIX semaphores provide a synchronization mechanism that works for both **threads** and **processes** (unlike mutexes, which are thread-only). Semaphores also support more complex synchronization beyond simple mutual exclusion.

---

## Basic Concepts

A semaphore is an integer ≥ 0 with two fundamental operations:

- **Decrement (lock)** — `sem_wait()`: succeeds if value > 0; blocks if value is 0
- **Increment (unlock)** — `sem_post()`: increments; may unblock a waiting thread/process

---

## Semaphore API (Unnamed Semaphores)

### `sem_init()`

```c
#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
```

| Parameter | Description |
|-----------|-------------|
| `sem` | Pointer to the semaphore |
| `pshared` | `0` = shared between threads; `1` = shared between processes |
| `value` | Initial value |

### `sem_destroy()`

```c
int sem_destroy(sem_t *sem);
```

Destroys the semaphore. Must not be called while threads are blocked on it.

### `sem_wait()`

```c
int sem_wait(sem_t *sem);
```

- If value > 0: decrements and returns immediately
- If value == 0: **blocks** until value becomes > 0, then decrements

### `sem_post()`

```c
int sem_post(sem_t *sem);
```

Increments the semaphore. If threads are blocked on `sem_wait()`, **one** is unblocked.

---

## Binary Semaphores

A semaphore initialized to **1** — works like a mutex:

1. Thread T1 calls `sem_wait()` → value becomes 0, T1 proceeds
2. Thread T2 calls `sem_wait()` → blocks (value is 0)
3. T1 calls `sem_post()` → value becomes 1, T2 is unblocked
4. T2 decrements to 0 and proceeds

### Exercise: Fix Race Condition with Semaphore

```c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_TO 10000000
#define NUM_THREADS 3

sem_t binary_sem;
int counter = 0;

void* perform_work(void* argument){
  for(int i = 0; i < COUNT_TO; i++){
    // Add sem_wait() and sem_post() around the critical section
    counter += 1;
  }
  return NULL;
}

int main(void){
  sem_init(&binary_sem, 0, 1);
  pthread_t threads[NUM_THREADS];

  for(int i = 0; i < NUM_THREADS; i++)
    pthread_create(&threads[i], NULL, perform_work, NULL);
  for(int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  sem_destroy(&binary_sem);
  printf("Expected value of counter = %d\n", COUNT_TO * NUM_THREADS);
  printf("Actual value of counter = %d\n", counter);
  exit(EXIT_SUCCESS);
}
```

---

## Counting Semaphores

Initialized to a value **greater than 1** — controls access to a **pool of resources**.

### Example: Sharing a Pool of Resources

```c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS  4
#define POOL_SIZE    2
#define ITERATIONS   5
#define SLEEP_DURATION 10

sem_t counting_sem;

void use_resource(){ sleep(SLEEP_DURATION); }

void* perform_work(void* argument){
  char thread_name = *((char*) argument);
  int resources_available;
  for(int i = 0; i < ITERATIONS; i++){
    printf("%c wants resource, iteration: %d\n", thread_name, i);
    sem_getvalue(&counting_sem, &resources_available);
    printf("Resources available: %d\n", resources_available);

    sem_wait(&counting_sem);
    printf("%c got resource\n", thread_name);
    use_resource();

    printf("%c giving up resource\n", thread_name);
    sem_post(&counting_sem);
  }
  return NULL;
}

int main(void){
  sem_init(&counting_sem, 0, POOL_SIZE);
  pthread_t threads[NUM_THREADS];
  char thread_name[NUM_THREADS];

  for(int i = 0; i < NUM_THREADS; i++){
    thread_name[i] = i + 65;  // 'A', 'B', 'C', 'D'
    pthread_create(&threads[i], NULL, perform_work, (void*) &thread_name[i]);
  }
  for(int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  sem_destroy(&counting_sem);
  exit(EXIT_SUCCESS);
}
```

When all `POOL_SIZE` resources are in use (semaphore = 0), additional threads **block** until a resource is released.

---

## Additional Resources

- [sem_overview — Linux man pages](https://man7.org/linux/man-pages/man7/sem_overview.7.html)
- Chapter 53 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
- [Chapter 31: Semaphores — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
- [Semaphore (Wikipedia)](https://en.wikipedia.org/wiki/Semaphore_(programming))
