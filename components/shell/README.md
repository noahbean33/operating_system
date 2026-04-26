# smallsh

A small Unix shell written in C and ARM assembly.

## Overview

**smallsh** implements a subset of common shell functionality, including:

- An interactive command prompt
- Built-in commands: `exit`, `cd`, and `status`
- Execution of external commands via `fork`/`exec`/`waitpid`
- Input and output redirection (`<` and `>`)
- Foreground and background process execution (`&`)
- Signal handling for `SIGINT` (Ctrl-C) and `SIGTSTP` (Ctrl-Z) with a foreground-only mode toggle

The project contains two implementations:

- **`src/main.c`** — Full-featured shell in C
- **`src/shell.s`** — Minimal shell in ARM assembly demonstrating low-level syscalls (`read`, `write`, `fork`, `execve`, `wait4`)

## Building

```bash
# C version
gcc --std=c23 -o smallsh src/main.c

# ARM assembly version (requires an ARM toolchain)
as -o shell.o src/shell.s && ld -o shell shell.o
```

## Usage

```
$ ./smallsh
: ls
: echo hello > out.txt
: cat < out.txt
: sleep 10 &
: exit
```
