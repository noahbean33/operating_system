# Exploration: Process API — Monitoring Child Processes

## Introduction

In many cases a parent process wants to know when and how its child process has terminated. Linux provides `wait()` and `waitpid()` for this purpose.

---

## `wait()`

```c
#include <sys/wait.h>

pid_t wait(int *wstatus);
```

- **Blocks** until any one child process terminates
- Returns the PID of the terminated child
- If a child has already terminated, returns immediately
- Termination status is stored in `*wstatus`

### Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
  pid_t spawnpid = -5;
  int childStatus;
  int childPid;

  spawnpid = fork();
  switch (spawnpid){
    case -1:
      perror("fork() failed!");
      exit(EXIT_FAILURE);
      break;
    case 0:
      printf("I am the child. My pid = %d\n", getpid());
      break;
    default:
      printf("I am the parent. My pid = %d\n", getpid());
      childPid = wait(&childStatus);
      printf("Parent's waiting is done as the child with pid %d exited\n", childPid);
      break;
  }
  printf("The process with pid %d is returning from main\n", getpid());
  return EXIT_SUCCESS;
}
```

---

## `waitpid()`

```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *wstatus, int options);
```

Addresses limitations of `wait()`:

- **`pid > 0`** — wait for the specific child with that PID
- **`pid == -1`** — wait for any child (like `wait()`)

### Example: Waiting for a Specific Child

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
  int childStatus;
  printf("Parent process's pid = %d\n", getpid());
  pid_t firstChild = fork();

  if(firstChild == -1){
    perror("fork() failed!");
    exit(EXIT_FAILURE);
  } else if(firstChild == 0){
    printf("First child's pid = %d\n", getpid());
    sleep(10);
  } else{
    pid_t secondChild = fork();
    if(secondChild == -1){
      perror("fork() failed!");
      exit(EXIT_FAILURE);
    } else if(secondChild == 0){
      printf("Second child's pid = %d\n", getpid());
      sleep(10);
    } else{
      pid_t childPid = waitpid(secondChild, &childStatus, 0);
      printf("The parent is done waiting. The pid of child that terminated is %d\n", childPid);
    }
  }
  printf("The process with pid %d is exiting\n", getpid());
  return EXIT_SUCCESS;
}
```

---

## Non-Blocking Wait Using `WNOHANG`

The `options` argument can include `WNOHANG`, making `waitpid()` **non-blocking**. If no child has terminated, it returns **0** immediately.

```c
childPid = waitpid(childPid, &childStatus, WNOHANG);
// Returns 0 if child hasn't terminated yet
```

---

## Interpreting the Termination Status

The `wstatus` value must be decoded using macros:

| Macro | Description |
|-------|-------------|
| `WIFEXITED(wstatus)` | Returns true if child terminated **normally** (via `exit()`) |
| `WEXITSTATUS(wstatus)` | Returns the exit status (only valid if `WIFEXITED` is true) |
| `WIFSIGNALED(wstatus)` | Returns true if child was terminated **by a signal** |
| `WTERMSIG(wstatus)` | Returns the signal number (only valid if `WIFSIGNALED` is true) |

### Example

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
  printf("Parent process's pid = %d\n", getpid());
  int   childStatus;
  pid_t childPid = fork();

  if(childPid == -1){
    perror("fork() failed!");
    exit(EXIT_FAILURE);
  } else if(childPid == 0){
    sleep(10);
  } else{
    printf("Child's pid = %d\n", childPid);
    childPid = waitpid(childPid, &childStatus, 0);
    printf("waitpid returned value %d\n", childPid);
    if(WIFEXITED(childStatus)){
      printf("Child %d exited normally with status %d\n", childPid, WEXITSTATUS(childStatus));
    } else{
      printf("Child %d exited abnormally due to signal %d\n", childPid, WTERMSIG(childStatus));
    }
  }
  return EXIT_SUCCESS;
}
```

---

## Zombie Processes

If a parent does not `wait()` for a terminated child, the child becomes a **zombie process**. Most resources are freed, but an entry remains in the process table. If the parent later calls `waitpid()`, the zombie is cleaned up.

If a parent terminates without cleaning its zombies, the **init process** adopts them and periodically reaps them.

---

## Additional Resources

- [wait and waitpid — Linux man pages](https://man7.org/linux/man-pages/man2/waitpid.2.html)
- Chapter 26 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
