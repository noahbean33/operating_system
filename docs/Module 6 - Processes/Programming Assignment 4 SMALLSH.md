# Programming Assignment 4: SMALLSH

> **Due:** Mar 2 by 11:59pm | **Points:** 170 | **Portfolio Assignment**

## Introduction

Write your own shell in C called **smallsh** that implements a subset of features of well-known shells such as bash:

- Provide a prompt for running commands
- Handle blank lines and comments (lines beginning with `#`)
- Execute 3 built-in commands: `exit`, `cd`, and `status`
- Execute other commands via `exec()` family of functions
- Support input and output redirection
- Support foreground and background processes
- Implement custom handlers for `SIGINT` and `SIGTSTP`

## Learning Outcomes

- Describe the Linux process API (Module 6, MLO 2)
- Write programs using the Linux process API (Module 6, MLO 3)
- Explain the concept of signals and their uses (Module 6, MLO 2)
- Write programs using the Linux API for signal handling (Module 7, MLO 3)
- Explain I/O redirection and write programs that employ it (Module 7, MLO 4)

---

## Program Functionality

### 1. The Command Prompt

Prompt symbol: `:` (colon)

```
command [arg1 arg2 ...] [< input_file] [> output_file] [&]
```

- Max command length: **2048 characters**, max **512 arguments**
- `<`, `>`, `&` must be surrounded by spaces
- `&` as the last word runs the command in the background

### 2. Comments & Blank Lines

- Lines beginning with `#` are comments — ignored
- Blank lines do nothing; re-prompt

### 3. Built-in Commands

**`exit`** — Kills all child processes/jobs, then terminates the shell.

**`cd`** — Changes working directory:
- No arguments → changes to `HOME` directory
- One argument → changes to specified path (absolute or relative)

**`status`** — Prints exit status or terminating signal of the last foreground process. Returns 0 if run before any foreground command.

> Built-in commands ignore `&`, don't support I/O redirection, and don't set the status value.

### 4. Executing Other Commands

- Use `fork()`, `exec()`, and `waitpid()`
- Use `PATH` variable to find commands
- On exec failure: print error, set exit status to 1, terminate child

### 5. Input & Output Redirection

- Use `dup2()` before `exec()`
- Input file: opened for **reading only**
- Output file: opened for **writing only** (truncate if exists, create if not)
- Errors: print message, set exit status to 1 (don't exit shell)

### 6. Foreground & Background Commands

**Foreground:** Shell waits for completion before next prompt.

**Background (command ends with `&`):**
- Shell returns to prompt immediately
- Print: `background pid is <pid>`
- On termination: `background pid <pid> is done: exit value <status>` (printed before next prompt)
- Redirect stdin/stdout to `/dev/null` if not explicitly redirected

### 7. Signals

**SIGINT (Ctrl-C):**
- Parent shell ignores it
- Background children ignore it
- Foreground child terminates itself
- Parent prints signal number if foreground child is killed

**SIGTSTP (Ctrl-Z):**
- Children (foreground & background) ignore it
- Toggles **foreground-only mode** in the shell:
  - `Entering foreground-only mode (& is now ignored)`
  - `Exiting foreground-only mode`

---

## Sample Execution

```
$ smallsh
: ls
junk   smallsh    smallsh.c
: ls > junk
: status
exit value 0
: cat junk
junk
smallsh
smallsh.c
: wc < junk > junk2
: wc < junk
       3       3      23
: test -f badfile
: status
exit value 1
: sleep 5
^Cterminated by signal 2
: sleep 15 &
background pid is 4923
: ps
  PID TTY          TIME CMD
 4923 pts/0    00:00:00 sleep
 4867 pts/0    00:01:32 smallsh
:
background pid 4923 is done: exit value 0
: cd
: pwd
/nfs/stak/users/chaudhrn
: ^C^Z
Entering foreground-only mode (& is now ignored)
: sleep 5 &
: ^Z
Exiting foreground-only mode
: exit
$
```

---

## Hints & Resources

1. **Command Prompt:** Call `fflush()` after every output
2. **Built-in Commands:** Program these first
3. **Executing Commands:** Use `execlp()` or `execvp()` (they search PATH)
4. **I/O Redirection:** Do redirection in the child process with `dup2()`
5. **Background:** Check for terminated background processes with `waitpid(..., WNOHANG)` before each prompt
6. **Signals:** `printf()` is **not reentrant** — use `write()` in signal handlers

---

## Submission

- Graded via **Gradescope**
- Submit: `youronid_assignment4.zip` or `youronid_assignment4.c`
- Compilation: `gcc --std=gnu99 -o smallsh *.c`
