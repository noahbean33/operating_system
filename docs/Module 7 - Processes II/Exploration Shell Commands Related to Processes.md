# Exploration: Shell Commands Related to Processes

## Introduction

The bash shell supports numerous commands that provide information and control over processes.

---

## Using `ps` to Get Process Information

### Basic `ps`

Displays processes running from the current terminal:

```
$ ps
  PID TTY          TIME CMD
19766 pts/18   00:00:00 bash
20096 pts/18   00:00:00 ps
```

### Augmented `ps` Command

```bash
ps -o ppid,pid,pgid,sid,euser,stat,%cpu,rss,args | head -n 1; \
ps -eH -o ppid,pid,pgid,sid,euser,stat,%cpu,rss,args | grep <username>
```

| Column | Description |
|--------|-------------|
| `PPID` | Parent process ID |
| `PID` | Process ID |
| `PGID` | Process Group ID |
| `SID` | Session ID |
| `EUSER` | Effective User |
| `STAT` | Process State |
| `%CPU` | CPU time percentage |
| `RSS` | Real Set Size (KB of RAM) |
| `COMMAND` | The command |

---

## Process State Codes

### Primary State Codes

| Char | Description | Module 6 State |
|------|-------------|---------------|
| `D` | Uninterruptible sleep (usually I/O) | Waiting |
| `R` | Running or runnable | Ready / Running |
| `S` | Interruptible sleep | Waiting |
| `T` | Stopped by job control signal | Waiting |
| `Z` | Zombie (terminated but not reaped) | Zombie |

### Additional State Characters

| Char | Description |
|------|-------------|
| `<` | High-priority (not nice) |
| `N` | Low-priority (nice) |
| `s` | Session leader |
| `+` | Foreground process group |

---

## Job Control

A **job** is a group of processes sharing the same process group ID. Job control allows starting multiple jobs from a single terminal and controlling their access.

### Foreground and Background Jobs

- **One** foreground job at a time (can interact with terminal)
- **Multiple** background jobs (limited terminal access)
- Start a background job with `&`:

```bash
$ ping oregonstate.edu &
```

### Job Control Commands

| Command | Description |
|---------|-------------|
| `jobs` | List jobs (`-l` to show PIDs) |
| `fg %n` | Bring job `n` to foreground |
| `bg %n` | Restart stopped job `n` in background |
| `Ctrl-Z` | Stop the foreground job (sends `SIGTSTP`) |
| `kill -TSTP %n` | Stop job `n` |

---

## Additional Resources

- Chapter 34 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
