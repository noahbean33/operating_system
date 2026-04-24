# Exploration: Condition Variables

## Introduction

A **condition variable** allows a thread to sleep until another thread signals that a condition is now true. This eliminates the inefficient busy-waiting of the mutex-only approach.

In the Producer-Consumer problem:
- **Consumer** sleeps when buffer is empty; producer signals when data is added
- **Producer** sleeps when buffer is full; consumer signals when space is freed

---

## Pthreads API for Condition Variables

```c
#include <pthread.h>

// Static initialization
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Wait on a condition variable (must hold mutex m)
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *m);

// Signal a condition variable (wake one waiting thread)
int pthread_cond_signal(pthread_cond_t *cond);
```

- `pthread_cond_wait()` — the calling thread **releases** the mutex and **blocks** on the condition variable. When signaled, it re-acquires the mutex before returning.
- `pthread_cond_signal()` — unblocks one thread waiting on the condition variable.

> **Note:** "Signal" here means "indicate" — unrelated to Unix signals.

---

## Example: Producer-Consumer with Condition Variables

```c
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SIZE 5
#define END_MARKER -1

int buffer[SIZE];
int count = 0;
int prod_idx = 0;
int con_idx = 0;
int num_items = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

int produce_item(int i){
  return (i == num_items) ? END_MARKER : rand() % 1000;
}

int put_item(int value){
  buffer[prod_idx] = value;
  prod_idx = (prod_idx + 1) % SIZE;
  count++;
  return value;
}

void *producer(void *args){
  for (int i = 0; i < num_items + 1; i++){
    int value = produce_item(i);
    pthread_mutex_lock(&mutex);
    while (count == SIZE)
      // Buffer full — wait for consumer to signal space available
      pthread_cond_wait(&empty, &mutex);
    put_item(value);
    // Signal consumer that buffer has data
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mutex);
    printf("PROD %d\n", value);
  }
  return NULL;
}

int get_item(){
  int value = buffer[con_idx];
  con_idx = (con_idx + 1) % SIZE;
  count--;
  return value;
}

void *consumer(void *args){
  int value = 0;
  while (value != END_MARKER){
    pthread_mutex_lock(&mutex);
    while (count == 0)
      // Buffer empty — wait for producer to signal data available
      pthread_cond_wait(&full, &mutex);
    value = get_item();
    // Signal producer that buffer has space
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    printf("CONS %d\n", value);
  }
  return NULL;
}

int main(int argc, char *argv[]){
  if (argc != 2){
    printf("Usage: ./main num_items\n");
    exit(1);
  }
  srand(time(0));
  num_items = atoi(argv[1]);
  pthread_t p, c;
  pthread_create(&p, NULL, producer, NULL);
  sleep(5);
  pthread_create(&c, NULL, consumer, NULL);
  pthread_join(p, NULL);
  pthread_join(c, NULL);
  return 0;
}
```

### Benefits

Using two condition variables (`empty` and `full`), the threads coordinate so that:

- The producer only puts data when there is space
- The consumer only gets data when there is data
- **Neither thread busy-waits** — they sleep and are woken only when conditions change

---

## Producer-Consumer with Unbounded Buffer

When the buffer is large enough to never be full, only **one condition variable** (`full`) is needed — the consumer waits on it and the producer signals it.

### Exercise

Given a buffer of size 15 and only 12 items + 1 end marker (13 total), the buffer will never be full. Modify the program to use only one condition variable.

---

## Summary

Condition variables provide efficient thread synchronization beyond mutual exclusion. Combined with a mutex, they allow threads to **sleep** until a relevant condition is true, avoiding wasted CPU cycles from polling.

---

## Additional Resources

- Chapter 30.2 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
- [Chapter 30: Condition Variables — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
- [Dining Philosophers Problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
