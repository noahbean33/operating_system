# Exploration: Directories

## Introduction

In this exploration, we study directories. We first look at directories and the API for manipulating them, then discuss how to get meta-data about files and directories.

---

## Directories

In Linux, a **directory** is a special file that contains entries about other files and directories. Each entry connects a human-readable filename to an **inode** (index node). An inode is a structure maintained by the OS's file system that contains meta-data about a file and the location of data blocks on disk.

### `mkdir()`

```c
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode);
```

Attempts to create a directory named `pathname`. The `mode` argument specifies the permissions.

### `rmdir()`

```c
#include <unistd.h>

int rmdir(const char *pathname);
```

Attempts to delete the directory at `pathname`. The directory **must be empty**.

---

## Opening, Reading, and Closing Directories

### `opendir()`

```c
#include <sys/types.h>
#include <dirent.h>

DIR *opendir(const char *name);
```

Opens the directory and returns a pointer to the **directory stream**, positioned at the first entry.

### `closedir()`

```c
#include <sys/types.h>
#include <dirent.h>

int closedir(DIR *dirp);
```

Closes the directory stream and the underlying file descriptor.

### `readdir()`

```c
#include <dirent.h>

struct dirent *readdir(DIR *dirp);
```

Returns a pointer to a `dirent` structure for the next directory entry. Returns `NULL` at the end of the directory stream.

The `dirent` structure has two POSIX-mandated fields:

```c
struct dirent {
  ino_t d_ino;       /* Inode number */
  char  d_name[256]; /* Null-terminated filename */
};
```

### Example: List Directory Entries

```c
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
  DIR* currDir = opendir(".");
  struct dirent *entry;

  while((entry = readdir(currDir)) != NULL){
    printf("%s  %lu\n", entry->d_name, entry->d_ino);
  }

  closedir(currDir);
  return 0;
}
```

---

## Getting File and Directory Meta-Data

### `stat()`

```c
#include <sys/stat.h>

int stat(const char *pathname, struct stat *statbuf);
```

Returns meta-data about a file or directory. Key fields of `struct stat`:

| Field | Type | Description |
|-------|------|-------------|
| `st_dev` | `dev_t` | ID of device containing file |
| `st_ino` | `ino_t` | Inode number |
| `st_mode` | `mode_t` | File type and permissions (bit mask) |
| `st_uid` | `uid_t` | User ID of owner |
| `st_gid` | `gid_t` | Group ID of owner |
| `st_size` | `off_t` | Total size in bytes |
| `st_atime` | `time_t` | Time of last access (seconds since Epoch) |
| `st_mtime` | `time_t` | Time of last modification (seconds since Epoch) |

### Example: Print File Modification Times

```c
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

int main(void){
  DIR* currDir;
  struct dirent *entry;
  struct stat dirStat;

  currDir = opendir(".");

  while((entry = readdir(currDir)) != NULL){
    stat(entry->d_name, &dirStat);
    printf("%s modified at %ld\n", entry->d_name, dirStat.st_mtime);
  }

  closedir(currDir);
  return 0;
}
```

### Determining File Type

Use standard macros that take `st_mode` as an argument:

- `S_ISREG()` — returns true for **regular files**
- `S_ISDIR()` — returns true for **directories**

See the [Linux man page for inode](https://man7.org/linux/man-pages/man7/inode.7.html) for more file type macros.

---

## Exercise

Modify the previous program so that it lists names and last modified time only for **regular files**.

---

## Additional Resources

- [Linux manual pages](https://man7.org/linux/man-pages/)
- Chapter 18 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
- [Files and Directories — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
