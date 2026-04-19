# Exploration: Inter-Process Communication

## Introduction

Linux provides various mechanisms for **Inter-Process Communication (IPC)**. We've already studied signals (limited IPC) and mutex locks (thread synchronization). This exploration surveys the breadth of IPC facilities.

---

## Categories of IPC

IPC facilities fall into two broad categories:

- **Communication facilities** — help processes exchange data
- **Synchronization facilities** — help processes/threads coordinate actions

---

## Communication Facilities

### IPC Using Data Transfers

One process writes data that another reads. Two models:

- **Byte streams** — writer writes bytes; reader reads arbitrary amounts
- **Messages** — writer writes complete messages; reader reads whole messages

#### Pipes

- **Unidirectional** data channel between related processes (common ancestor via `fork()`)
- Exchange data as **byte streams**
- Parent writes → child reads, or vice versa

#### FIFO (Named Pipes)

- Similar to pipes, but have a **name** in the filesystem
- Can be used between **unrelated** processes on the same machine
- Exchange data as **byte streams**
- Can have multiple readers and writers

#### Message Queues

- Exchange data as **messages** with type, length, and payload
- Messages can be fetched FIFO or by type field

#### Sockets

- Can be used on the **same machine** or across **different machines**
- **Datagram sockets** — message-based
- **Stream sockets** — byte streams

---

### IPC Using Shared Memory

Special memory segments shared among multiple processes. Processes exchange data by reading/writing to this shared region.

- **Memory-mapped files** — map part/all of a file to memory using `mmap()`, then access contents as a buffer instead of using `read()`/`write()`

### Comparing Data Transfer vs. Shared Memory

| Aspect | Data Transfer | Shared Memory |
|--------|:------------:|:-------------:|
| **Speed** | Slower (kernel copies) | Faster (no syscalls after setup) |
| **Synchronization** | Built-in (read blocks if empty) | Must use semaphores, etc. |
| **Readers** | One process reads the data | Multiple processes can read |

---

## Synchronization Facilities

### Semaphores

An integer ≥ 0 with two operations:
- **Decrement** — blocks if value is 0
- **Increment** — may unblock a waiting process

**Binary semaphore** (0 or 1) — enforces mutual exclusion
**Counting semaphore** — controls access to a pool of resources

### Mutexes and Condition Variables

- Synchronize **threads** within one process
- Cannot be used across multiple processes

### File Locks

- Coordinate file operations by multiple processes
- Can lock entire files or portions of a file

---

## Additional Resources

- Chapter 43 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
