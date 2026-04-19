# Exploration: Synchronization Mechanisms Beyond Mutex

## Introduction

Mutexes prevent multiple threads from accessing a shared resource simultaneously. But sometimes threads need to **wait until a condition is true** and be **notified** when it becomes true. This motivates **condition variables**, explored in the next exploration. Here we introduce the **Producer-Consumer Problem**.

---

## Producer-Consumer Problem

Also called the **bounded buffer problem**:

- **Producer** generates data and puts it in a shared buffer
- **Consumer** picks data from the buffer and processes it
- If buffer is **full**, producer must wait
- If buffer is **empty**, consumer must wait

### Real-World Examples

- **Unix pipe (`|`):** `cat foo.txt | more` — `cat` is the producer, `more` is the consumer
- **Multi-threaded web servers:** producer thread queues HTTP requests; consumer threads process them

---

## Solving with Mutex (Inefficient)

The producer and consumer use a mutex. When the buffer is full/empty, they **sleep** and retry:

```c
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 5
#define END_MARKER -1

int buffer[SIZE];
int count = 0;
int prod_idx = 0, con_idx = 0;
int num_items = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
    while (count == SIZE){
      pthread_mutex_unlock(&mutex);
      sleep(1);
      pthread_mutex_lock(&mutex);
    }
    put_item(value);
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
    while (count == 0){
      pthread_mutex_unlock(&mutex);
      sleep(1);
      pthread_mutex_lock(&mutex);
    }
    value = get_item();
    pthread_mutex_unlock(&mutex);
    printf("CONS %d\n", value);
  }
  return NULL;
}

int main(int argc, char *argv[]){
  if (argc != 2){ printf("Usage: ./main num_items\n"); exit(1); }
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

### Why This is Inefficient

- When buffer is full, the producer **busy-waits** (sleep + re-check)
- When buffer is empty, the consumer **busy-waits**
- Increasing sleep time degrades response; decreasing it wastes CPU

---

## Thread Termination Strategy

The producer places a special **END_MARKER** (`-1`) in the buffer before terminating. The consumer terminates when it reads the marker. This ensures all buffered items are consumed before the program exits.

---

## Summary

Using a mutex alone can enforce producer-consumer requirements but leads to **inefficient busy waiting**. Condition variables (next exploration) solve this problem.

---

## Additional Resources

- [Chapter 30: Condition Variables — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
