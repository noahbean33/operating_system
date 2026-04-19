# Exploration: Server Design

## Introduction

When implementing a server, we want **low response time**, **high throughput**, **maximum CPU utilization**, and **ease of design**. This exploration examines various server architectures.

---

## Iterative Servers

Handle **one client at a time**. Additional clients must wait for all previous requests to complete.

- **Pros:** Easy to design, implement, and maintain
- **Cons:** Poor response time under load; doesn't maximize CPU utilization (blocks on I/O)

---

## Concurrent Servers

Service (or appear to service) **multiple client connections simultaneously**.

### Real Concurrency

#### 1. One Process Per Client Connection

Fork a new process for each connection.

- **Pros:** Simple design, minimal shared state
- **Cons:** `fork()` is slow; context-switching overhead

#### 2. Pool of Processes

Pre-create a pool of processes; assign idle processes to incoming connections.

- **Pros:** Rapid response (no fork overhead)
- **Cons:** Complex pool management; context-switching overhead

#### 3. One Thread Per Client Connection

Create a new thread for each connection.

- **Pros:** Thread creation and context-switching much faster than processes
- **Cons:** Must be thread-safe; risk of inadvertent data sharing

#### 4. Pool of Threads

Pre-create a pool of threads; assign idle threads to incoming connections.

- **Pros:** Rapid response (no thread creation overhead)
- **Cons:** Thread-safety, data sharing, and pool management complexity

### Apparent Concurrency (I/O Multiplexing)

A single thread switches between clients when I/O would block. Increases CPU utilization but adds complexity.

---

## `select()` for I/O Multiplexing

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

Monitors multiple file descriptors, blocking until one or more become ready for I/O.

### Arguments

| Argument | Description |
|----------|-------------|
| `nfds` | Highest-numbered FD + 1 |
| `readfds` | FDs to watch for read readiness |
| `writefds` | FDs to watch for write readiness |
| `exceptfds` | FDs to watch for exceptions |
| `timeout` | Max time to block |

### Returns

- **Positive** — number of ready FDs
- **0** — timeout expired
- **-1** — error

### `fd_set` Macros

| Macro | Description |
|-------|-------------|
| `FD_ZERO(&set)` | Clear all bits |
| `FD_SET(fd, &set)` | Set bit for `fd` |
| `FD_CLR(fd, &set)` | Clear bit for `fd` |
| `FD_ISSET(fd, &set)` | Test if bit for `fd` is set |

### Example: Watching stdin

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TIMEOUT_SEC 25

int main(void){
  fd_set readFDs;
  struct timeval timeToWait;

  FD_ZERO(&readFDs);
  FD_SET(0, &readFDs);

  timeToWait.tv_sec = TIMEOUT_SEC;
  timeToWait.tv_usec = 0;

  int retval = select(1, &readFDs, NULL, NULL, &timeToWait);

  if (retval == -1)
    perror("select()");
  else if (retval)
    printf("Data is available now!\n");
  else
    printf("No data within %d seconds\n", TIMEOUT_SEC);

  return 0;
}
```

---

## Additional Resources

- Chapters 60 and 63 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
