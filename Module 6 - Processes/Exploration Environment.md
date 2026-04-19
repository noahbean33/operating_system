# Exploration: Environment

## Introduction

Each process has an associated **array of strings** called the process's **environment**. Each string is a text variable in the form `name=value`, and these names are called **environment variables**.

---

## Environment Variables and the Shell

View environment variables in bash with `printenv`:

```
SHELL=/bin/bash
HOSTNAME=os1.engr.oregonstate.edu
PWD=/nfs/stak/users/chaudhrn
HOME=/nfs/stak/users/chaudhrn
USER=chaudhrn
PATH=/class/local/bin:/usr/local/bin:/usr/bin:/usr/sbin
CC=gcc
CFLAGS=-std=gnu99 -Wall
```

Notable variables:

| Variable | Description |
|----------|-------------|
| `PATH` | Colon-separated list of directories searched for commands |
| `HOME` | Pathname of the user's login directory |
| `PWD` | Pathname of the current working directory |

---

## Setting & Unsetting Variables in Bash

```bash
# Set a variable
export MYVAR=foo

# Unset a variable
unset MYVAR

# Print a variable
echo $PATH

# Append to a variable
export PATH=$PATH:.
```

---

## Getting and Setting Environment Variables in C

### `getenv()`

```c
#include <stdlib.h>

char *getenv(const char *name);
```

Returns a pointer to the value of the variable, or `NULL` if not found.

### `setenv()`

```c
#include <stdlib.h>

int setenv(const char *name, const char *value, int overwrite);
```

- If `overwrite` is 0 and the variable exists, the value is **not** changed
- If `overwrite` is non-zero, the value is changed regardless
- Creates copies of `name` and `value`

### `unsetenv()`

```c
#include <stdlib.h>

int unsetenv(const char *name);
```

Deletes the variable from the environment.

---

## Environment Variables Across Parent and Child Processes

When a parent forks a child, the child **inherits** the parent's environment. However, after forking, parent and child have **independent** environments — changes in one do not affect the other.

### Example

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int childStatus;
    char *varName = "MYVAR";
    setenv(varName, "foo", 1);

    printf("%s in parent is %s\n", varName, getenv(varName));
    pid_t spawnPid = fork();
    switch (spawnPid){
    case -1:
        perror("fork() failed!");
        exit(1);
        break;
    case 0:
        printf("%s in child is %s\n", varName, getenv(varName));
        setenv(varName, "bar", 1);
        printf("%s in child has been updated to %s\n", varName, getenv(varName));
        break;
    default:
        spawnPid = waitpid(spawnPid, &childStatus, 0);
        printf("%s in parent is still %s\n", varName, getenv(varName));
        break;
    }
    return 0;
}
```

Output demonstrates that changing `MYVAR` in the child to `"bar"` does **not** affect the parent's value of `"foo"`.

---

## Additional Resources

- [environ — Linux man page](https://man7.org/linux/man-pages/man7/environ.7.html)
- Chapter 6 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
