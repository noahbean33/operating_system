# Exploration: Concurrency

## Multiprogramming and Multiprocessing

An OS can switch the CPU between multiple ready processes, giving the impression of simultaneous execution — this is **multiprogramming**. The **scheduler** decides which process runs on the CPU.

- The scheduler may switch processes when the running process **blocks** (e.g., waiting for I/O)
- The scheduler may also switch if a process has been running **too long**
- On systems with **multiple CPUs** (**multiprocessing**), many processes can truly run at the same time

### Context Switching

When one process is moved off the CPU and another takes its place:

1. OS **stores** the registers and program counter of the removed process
2. OS **restores** the registers and program counter of the new process

Context switching is **expensive** — the CPU does bookkeeping rather than useful work during the switch.

---

## Potential Pitfalls of Concurrency

Problems arise when concurrent processes **access and update the same data**.

### Example: ATM Withdrawal

Consider an account balance of $100, with $20 withdrawn simultaneously from two ATMs:

| Account Balance | Process 1 | Process 2 |
|:-:|---|---|
| $100 | | |
| $100 | Reads $100 into `balance` | |
| $100 | | Reads $100 into `balance` |
| $100 | Subtracts $20 → `balance` = $80 | |
| $100 | | Subtracts $20 → `balance` = $80 |
| $80 | Writes $80 to file | |
| $80 | | Writes $80 to file |

**Result:** $40 was withdrawn but balance is $80 instead of $60!

---

## Race Conditions, Critical Sections, and Mutual Exclusion

- **Race condition** — the result depends on the timing of concurrent operations on a shared resource
- **Critical section** — the part of code that accesses or modifies a shared resource
- **Mutual exclusion** — when one process is in its critical section, no other process accessing the same resource should execute its critical section

To enforce mutual exclusion, a **lock** mechanism is needed — only one process can hold the lock at a time.

---

## Concurrency Within a Program

Even a single process can benefit from concurrent execution. Consider a browser:

- A single tab may need multiple network requests (e.g., downloading images)
- While waiting for one download, the process could render a previously downloaded image
- Multiple tabs open simultaneously create additional concurrency opportunities

### Approaches

- **Multiple processes** — cooperating processes with IPC (shared memory, pipes, etc.)
- **Threads** — simpler and less resource-intensive mechanism for in-process concurrency

---

## Additional Resources

- [Therac-25 (Wikipedia)](https://en.wikipedia.org/wiki/Therac-25) — infamous real-world disaster caused by concurrency bugs in a radiation therapy machine
