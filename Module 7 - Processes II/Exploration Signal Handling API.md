# Exploration: Signal Handling API

## Introduction

When a process receives a signal, the default action occurs. However, if the signal is **catchable**, we can register a **custom signal handler** to override the default action.

---

## Signal Sets

A **signal set** is a list of signal types defined using `sigset_t`:

```c
sigset_t my_signal_set;
```

### Signal Set Functions

| Function | Signature | Description |
|----------|-----------|-------------|
| `sigemptyset` | `int sigemptyset(&set)` | Initialize set to empty |
| `sigfillset` | `int sigfillset(&set)` | Initialize set with all signal types |
| `sigaddset` | `int sigaddset(&set, signal)` | Add a signal to the set |
| `sigdelset` | `int sigdelset(&set, signal)` | Remove a signal from the set |

---

## `sigaction()` Function & Structure

### The Function

```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

- **`signum`** — the signal type (any except `SIGKILL` and `SIGSTOP`)
- **`act`** — pointer to a struct describing the action to take
- **`oldact`** — if not `NULL`, filled with the previous handling settings

### The Structure

```c
struct sigaction {
  void (*sa_handler)(int);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_sigaction)(int, siginfo_t*, void*);
};
```

**`sa_handler`** — pointer to the signal handler function, or:
- `SIG_DFL` — use the default action
- `SIG_IGN` — ignore the signal

**`sa_mask`** — signals to **block** while the handler executes (held and delivered after handler completes)

**`sa_flags`** — additional instructions:
- `0` — no special flags
- `SA_RESETHAND` — reset handler to `SIG_DFL` after first signal
- `SA_SIGINFO` — use `sa_sigaction` instead of `sa_handler`
- `SA_RESTART` — automatically restart interrupted system calls

**`sa_sigaction`** — alternative handler function (used with `SA_SIGINFO`)

---

## Using `pause()` to Wait for Signals

```c
#include <unistd.h>

int pause(void);
```

Suspends execution until a signal is received:
- Ignored signal → `pause()` continues
- Terminating signal → process dies
- Caught signal → handler executes, then `pause()` returns -1

---

## Examples

### No Custom Handler

```c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
  printf("Send SIGINT by entering Control-C\n");
  fflush(stdout);
  pause();
  printf("pause() ended. The process will now end.\n");
  return 0;
}
```

Ctrl-C terminates the process via the default `SIGINT` handler.

### Custom Handler for SIGINT

```c
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handle_SIGINT(int signo){
  char* message = "Caught SIGINT, sleeping for 10 seconds\n";
  write(STDOUT_FILENO, message, 39);
  sleep(10);
}

int main(){
  struct sigaction SIGINT_action = {0};

  SIGINT_action.sa_handler = handle_SIGINT;
  sigfillset(&SIGINT_action.sa_mask);
  SIGINT_action.sa_flags = 0;

  sigaction(SIGINT, &SIGINT_action, NULL);

  printf("Send SIGINT by entering Control-C.\n");
  fflush(stdout);
  pause();
  printf("pause() ended. The process will now end.\n");
  return 0;
}
```

### Multiple Handlers and Ignoring Signals

```c
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void handle_SIGINT(int signo){
  char* message = "Caught SIGINT, sleeping for 10 seconds\n";
  write(STDOUT_FILENO, message, 39);
  raise(SIGUSR2);
  sleep(10);
}

void handle_SIGUSR2(int signo){
  char* message = "Caught SIGUSR2, exiting!\n";
  write(STDOUT_FILENO, message, 25);
  exit(0);
}

int main(){
  struct sigaction SIGINT_action = {0}, SIGUSR2_action = {0}, ignore_action = {0};

  SIGINT_action.sa_handler = handle_SIGINT;
  sigfillset(&SIGINT_action.sa_mask);
  SIGINT_action.sa_flags = 0;
  sigaction(SIGINT, &SIGINT_action, NULL);

  SIGUSR2_action.sa_handler = handle_SIGUSR2;
  sigfillset(&SIGUSR2_action.sa_mask);
  SIGUSR2_action.sa_flags = 0;
  sigaction(SIGUSR2, &SIGUSR2_action, NULL);

  // Ignore SIGTERM, SIGHUP, SIGQUIT
  ignore_action.sa_handler = SIG_IGN;
  sigaction(SIGTERM, &ignore_action, NULL);
  sigaction(SIGHUP, &ignore_action, NULL);
  sigaction(SIGQUIT, &ignore_action, NULL);

  while(1){ pause(); }
  return 0;
}
```

---

## Reentrant Functions

Signal handlers interrupt normal execution. If the handler calls a function that uses **global data structures**, it may corrupt state. Functions safe to use in signal handlers are called **reentrant**.

- `printf()` is **NOT reentrant** — use `write()` instead
- `strlen()` is **NOT reentrant** — use hard-coded lengths
- `strtok_r()` is reentrant; `strtok()` is not

```c
// In signal handlers, use write():
char* message = "Caught SIGUSR2, exiting!\n";
write(STDOUT_FILENO, message, 25);
```

---

## Signals and Interrupted Functions

When system calls (e.g., `getline()`) are interrupted by signals, they may return errors.

**Solution 1:** Use `SA_RESTART` flag:
```c
SIGTSTP_action.sa_flags = SA_RESTART;
```

**Solution 2:** Check for errors and reset:
```c
int numChars = getline(&userInput, &bufferSize, stdin);
if (numChars == -1){
  clearerr(stdin);
}
```

---

## Child Processes and Signal Inheritance

- `fork()` — child inherits signal handlers from parent
- `exec()` — **removes** custom signal handlers (resets to defaults), but `SIG_DFL` and `SIG_IGN` are preserved

---

## Additional Resources

- Chapter 21 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
