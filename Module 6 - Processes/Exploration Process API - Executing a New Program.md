# Exploration: Process API — Executing a New Program

## Introduction

`fork()` creates a new child process, but it still runs the same program as the parent. To run a **new program**, we use one of the `exec()` family of functions.

---

## The `exec` Family of Functions

The `exec` functions replace a running program with a new program. Calling `exec()` does **not** change the process ID, but replaces the text, data, heap, and stack segments.

> **Note:** There is no function named `exec()` itself — the name refers to the family of functions.

```c
#include <unistd.h>

int execl(const char *pathname, const char *arg, ... /* (char *) NULL */);
int execlp(const char *filename, const char *arg, ... /* (char *) NULL */);
int execle(const char *pathname, const char *arg, ... /*, (char *) NULL, char *const envp[] */);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *filename, char *const argv[]);
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

---

## `execv()`

```c
int execv(const char *pathname, char *const argv[]);
```

- `pathname` — path to the executable (absolute or relative)
- `argv` — array of strings; `argv[0]` must be the pathname, last element must be `NULL`

### Example: Running `ls -al`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  char *newargv[] = { "/bin/ls", "-al", NULL };
  execv(newargv[0], newargv);

  /* exec returns only on error */
  perror("execv");
  exit(EXIT_FAILURE);
}
```

### When `exec()` Fails

An `exec()` function returns to the calling program **only on error**. On success, the original program is completely replaced and never resumes.

---

## Using `exec()` with `fork()`

A common pattern (e.g., for a shell):

1. Parent displays a prompt
2. User enters a command → parent calls `fork()`
3. Child uses `exec()` to run the command
4. Parent calls `waitpid()` to wait for the child
5. When child terminates, parent loops back to the prompt

### Example

```c
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  char *newargv[] = { "/bin/ls", "-al", NULL };
  int childStatus;

  pid_t spawnPid = fork();

  switch(spawnPid){
  case -1:
    perror("fork()\n");
    exit(1);
    break;
  case 0:
    printf("CHILD(%d) running ls command\n", getpid());
    execv(newargv[0], newargv);
    perror("execve");
    exit(2);
    break;
  default:
    spawnPid = waitpid(spawnPid, &childStatus, 0);
    printf("PARENT(%d): child(%d) terminated. Now parent is exiting\n", getpid(), spawnPid);
    exit(0);
    break;
  }
}
```

---

## Differences Between `exec` Functions

| Function | Searches PATH? | Arguments | Custom Environment? |
|----------|:-------------:|:---------:|:------------------:|
| `execl()` | No | List | No |
| `execle()` | No | List | Yes |
| `execlp()` | Yes | List | No |
| `execv()` | No | Array | No |
| `execve()` | No | Array | Yes |
| `execvp()` | Yes | Array | No |

**Key differences:**
- **`p` suffix** — searches the `PATH` environment variable for the executable
- **`l` vs `v`** — `l` = arguments as a list; `v` = arguments as an array (vector)
- **`e` suffix** — allows passing a custom environment (`envp[]`)

### `execl()` Example

```c
execl("/bin/ls", "/bin/ls", "-al", NULL);
```

### `execlp()` Example

```c
execlp("ls", "ls", "-al", NULL);  // Searches PATH, no need for full path
```

---

## Additional Resources

- Chapter 27 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
