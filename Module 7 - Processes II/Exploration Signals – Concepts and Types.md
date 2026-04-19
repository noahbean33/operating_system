# Exploration: Signals — Concepts and Types

## Introduction

A **signal** is a mechanism for one process to notify another process of an event. Signals are used by the OS kernel to notify processes, as well as by user processes to notify other user processes.

---

## Basic Concepts

Signals are often termed **software interrupts** — they interrupt the normal flow of control and cause the process to jump to execute a **signal handler** function. The OS provides default signal handlers for all signals, but also supports registering **custom signal handlers** for most signals.

### Example

1. Running `ping` from a shell
2. Enter **Ctrl-Z** → sends `SIGTSTP` to the process
3. The default handler for `SIGTSTP` stops the program and returns control to the shell

There is a **fixed set of signals** — we cannot create our own, but we can control the programmatic response to most of them.

---

## Uses for Signals

**From the OS kernel to a process:**
- Process made an invalid memory reference
- A timer has expired
- A child process has completed
- A terminal event occurred (e.g., Ctrl-Z)
- A communicating process has died

**From one user process to another:**
- Resume, suspend, or terminate execution
- Change communication method (e.g., switch ports)

---

## Sending Signals from the Shell

```bash
kill -WXYZ PID
```

- `WXYZ` is the signal name (default: `TERM`)
- `PID` is the target process ID

---

## Signal Types & Default Actions

### Signals Related to Process Termination

| Signal | # | Easy Name | Catchable | Default Action | Core Dump |
|--------|---|-----------|-----------|---------------|-----------|
| `SIGABRT` | 6 | Abort | Yes | Terminate | Yes |
| `SIGINT` | 2 | Interrupt | Yes | Terminate | No |
| `SIGKILL` | 9 | Kill | **No** | Terminate | No |
| `SIGQUIT` | 3 | Quit | Yes | Terminate | Yes |
| `SIGTERM` | 15 | Terminate | Yes | Terminate | No |

- **SIGABRT** — sent by `abort()`; no cleanup performed
- **SIGINT** — sent by Ctrl-C to the foreground process group
- **SIGKILL** — always terminates; cannot be caught, blocked, or ignored
- **SIGQUIT** — sent by Ctrl-\\; creates a core dump
- **SIGTERM** — catchable termination; allows cleanup (preferred over `SIGKILL`)

### Signals Notifying of Errors

| Signal | # | Easy Name | Catchable | Default Action | Core Dump |
|--------|---|-----------|-----------|---------------|-----------|
| `SIGBUS` | - | Bus Error | Yes | Terminate | Yes |
| `SIGFPE` | 8 | Floating Point Error | Yes | Terminate | Yes |
| `SIGILL` | 4 | Illegal Instruction | Yes | Terminate | Yes |
| `SIGPIPE` | 13 | Pipe | Yes | Terminate | Yes |
| `SIGSEGV` | 11 | Segmentation Fault | Yes | Terminate | Yes |
| `SIGSYS` | - | System Call | Yes | Terminate | Yes |

### Signals Related to Control

| Signal | # | Easy Name | Catchable | Default Action | Core Dump |
|--------|---|-----------|-----------|---------------|-----------|
| `SIGALARM` | 14 | Alarm | Yes | Terminate | No |
| `SIGCONT` | - | Continue | Yes | Continue | - |
| `SIGHUP` | 1 | Hang Up | Yes | Terminate | No |
| `SIGSTOP` | - | Stop | **No** | Stop | - |
| `SIGTSTP` | - | Terminal Stop | Yes | Stop | - |
| `SIGTRAP` | 5 | Trap | Yes | Terminate | No |

- **SIGCONT** — resumes a stopped process; ignored if not stopped
- **SIGSTOP** — stops a process; not catchable
- **SIGTSTP** — Ctrl-Z; stops the foreground process group

### SIGCHLD

| Signal | # | Easy Name | Catchable | Default Action | Core Dump |
|--------|---|-----------|-----------|---------------|-----------|
| `SIGCHLD` | - | Child Terminated | Yes | None | - |

Sent by the kernel when a child process terminates, stops, or resumes. Allows a parent to do other work and call `wait()`/`waitpid()` when ready.

### Signals for Programmer Use

| Signal | # | Easy Name | Catchable | Default Action | Core Dump |
|--------|---|-----------|-----------|---------------|-----------|
| `SIGUSR1` | - | User 1 | Yes | Terminate | No |
| `SIGUSR2` | - | User 2 | Yes | Terminate | No |

These have **no meaning to the kernel** — provided for custom use by programmers.

---

## Additional Resources

- [Signal overview — Linux man pages](https://man7.org/linux/man-pages/man7/signal.7.html)
- Chapter 20 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
