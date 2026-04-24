# Exploration: Permissions

## Introduction

In this exploration we discuss file and directory permissions in Linux, and how to specify permissions when creating a file or directory.

---

## File & Directory Permissions

Permissions are managed in **3 scopes**: **owner/user**, **group**, and **others**. For each scope, 3 types of permissions can be granted:

### File Permissions

| Permission | Description |
|-----------|-------------|
| **Read** | Allows reading the file (e.g., `cat`, `open()` for reading) |
| **Write** | Allows modifying the file |
| **Execute** | Allows executing the file as a program |

### Directory Permissions

| Permission | Description |
|-----------|-------------|
| **Read** | Allows reading directory contents (e.g., `ls`) |
| **Write** | Allows creating, deleting, renaming files (requires execute too) |
| **Execute** | Allows accessing file content and meta-data by name |

This means there are **9 permissions** total (3 scopes × 3 types).

### Viewing Permissions with `ls -l`

```
$ ls -l
total 20
-rw-------. 1 chaudhrn upg11000 1147 Apr 19 11:39 catch-signals.c
-rwxr-x---. 1 chaudhrn upg11000  228 Apr 19 11:39 sigtest.sh
```

Permissions are displayed as 3 sets of 3 characters:
1. **First set** — owner
2. **Second set** — group
3. **Third set** — others

In each set: `r` = read, `w` = write, `x` = execute, `-` = no permission.

The character before owner permissions indicates file type: `-` for regular file, `d` for directory.

---

## Octal Notation for Permissions

Each scope is represented by **one octal digit** (base 8):

| Permission | Adds |
|-----------|------|
| Read | **4** |
| Write | **2** |
| Execute | **1** |

### Single-Scope Values

| Symbolic | Octal | Description |
|----------|-------|-------------|
| `rwx` | 7 | Read, write, & execute |
| `rw-` | 6 | Read & write |
| `r-x` | 5 | Read & execute |
| `r--` | 4 | Read only |
| `-wx` | 3 | Write & execute |
| `-w-` | 2 | Write only |
| `--x` | 1 | Execute only |
| `---` | 0 | No permissions |

### Full Permission Examples

| Symbolic | Octal | Description |
|----------|-------|-------------|
| `----------` | `0000` | No permissions |
| `-rwx------` | `0700` | Owner: rwx |
| `-rwxrwx---` | `0770` | Owner & group: rwx |
| `-rwxrwxrwx` | `0777` | All: rwx |
| `-rw-rw-rw-` | `0666` | All: read & write |
| `-rwxr-----` | `0740` | Owner: rwx; group: read |

---

## `mode` Argument in `open()` and `mkdir()`

Both system calls accept a `mode` argument using octal notation:

```c
file_descriptor = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, 0600);
```

`0600` corresponds to `rw-------` — owner can read and write; no permissions for group or others.

---

## Changing Permissions with `chmod`

- **Shell command:** [`chmod`](https://man7.org/linux/man-pages/man1/chmod.1.html)
- **System call:** [`chmod()`](https://man7.org/linux/man-pages/man2/chmod.2.html)

---

## `umask`: File Mode Creation Mask

Each process has an associated **umask** value that specifies permissions to **deny** when creating files/directories. This guards against inadvertently granting dangerous permissions.

**Example:** If `mode = 0777` and `umask = 0007`, the resulting permissions are `0770`.

### Viewing and Setting `umask`

```bash
$ umask
0007
```

System call:

```c
#include <sys/stat.h>

mode_t umask(mode_t mask);
```

---

## Additional Resources

- [File-system permissions (Wikipedia)](https://en.wikipedia.org/wiki/File-system_permissions)
- Chapter 15 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
