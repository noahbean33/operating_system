# Exploration: Process Concept & States

## Introduction

A **program** is executable code. A **process** is an instance of a running program. Process management is a key functionality of an OS.

---

## Process ID

In Linux, each process has a unique identity called the **process ID** (pid) — a non-negative integer. At any given time, all processes have unique IDs, but when a process terminates its ID can be reused.

```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);   // Returns PID of current process
pid_t getppid(void);  // Returns PID of parent process
```

### Example

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  printf("My pid is %d\n", getpid());
  printf("My parent's pid is %d\n", getppid());
  return 0;
}
```

The process with ID **1** is generally the **init** process, started by the OS during startup.

---

## Process API

Linux provides an API for processes including system calls for:

### Creating Processes

- When we open a new shell or run a command, the OS creates a new process
- Each process has its own **memory space** (code, stack, heap)
- Relevant system calls: `fork()`, `exec()` family

### Terminating Processes

- Many processes stop by themselves when complete (e.g., `ls`)
- The OS can also terminate processes when required

### Controlling & Monitoring Processes

- `wait()` and `waitpid()` allow a process to wait for another to finish
- `getpriority()` / `setpriority()` — get or change process priority
- `times()` — get CPU time statistics

---

## Process States

Modern OSs provide **multi-programming** — keeping multiple processes ready to run, giving the impression of simultaneous execution. The three primary states are:

### Ready

The process is ready to run on the CPU, but the OS has allocated the CPU to another process.

### Running

The process is actively executing instructions on the CPU.

### Waiting

The process is waiting for some event (e.g., I/O completion) before it can be ready again.

### State Transitions

- **Ready → Running:** OS schedules the process to the CPU
- **Running → Ready:** OS unschedules the process from the CPU
- **Running → Waiting:** Process initiates a blocking action (e.g., I/O call)
- **Waiting → Ready:** The blocking action completes (e.g., I/O done)

---

## Additional Resources

- [The Abstraction: The Process — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
