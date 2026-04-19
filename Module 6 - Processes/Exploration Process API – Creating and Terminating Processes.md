# Exploration: Process API — Creating and Terminating Processes

## Introduction

In this exploration, we look at the Unix API for creating new processes and terminating processes.

---

## Creating Processes Using `fork()`

```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```

`fork()` creates a new **child process** that is almost an exact duplicate of the **parent process**. The child has its own memory space (a copy of the parent's). The key difference:

- `fork()` returns **0** in the child process
- `fork()` returns the **child's PID** in the parent process
- `fork()` returns **-1** on failure

### Example

```c
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    pid_t spawnpid = -5;
    int intVal = 10;

    spawnpid = fork();
    switch (spawnpid){
        case -1:
            perror("fork() failed!");
            exit(1);
            break;
        case 0:
            intVal = intVal + 1;
            printf("I am the child! intVal = %d\n", intVal);
            break;
        default:
            intVal = intVal - 1;
            printf("I am the parent! intVal = %d\n", intVal);
            break;
    }
    printf("This statement will be executed by both of us!\n");
}
```

The child and parent have **separate memory spaces**. Changes to variables in one process do not affect the other.

> **Note:** The order of parent/child output may vary across runs due to OS scheduling.

---

## Tree of Processes

When a process calls `fork()`, the child may in turn call `fork()`, creating a **tree structure** of parent-child relationships.

---

## What Does a Child Inherit?

- Open file descriptors
- Current working directory
- Process group ID
- Environment variables
- Resource limits
- Signal handling settings

---

## Failure of `fork()`

If `fork()` fails (returns -1), it is usually because there are too many processes already running or resource limits would be exceeded.

---

## Terminating Processes

### Normal Termination

The process exits itself:

- **Case 1:** Successful completion — `return 0;` from `main()` (internally calls `exit(0)`)
- **Case 2:** Error recognized — process calls `exit()` with a non-zero status

```c
#include <stdlib.h>

void exit(int status);
```

By convention: `EXIT_SUCCESS` (0) for success, `EXIT_FAILURE` for errors. The exit status is transmitted to the parent process.

`exit()` calls registered exit handlers (`atexit()`), flushes I/O streams, then calls `_exit()` to clean up.

### Abnormal Termination

A process receives a **signal** that causes it to terminate (discussed in Module 7).

### Orphaned Child Processes

If a parent terminates while children are still running, the **init process** becomes the new parent of the orphaned children.

---

## Additional Resources

- [The Process API — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
- Chapters 24–25 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
