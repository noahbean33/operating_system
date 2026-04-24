# Exploration: Files

## Introduction

Files are key abstractions provided by an OS. For a system programmer, a file is a **stream of bytes** that can be accessed as a linear array. In Linux, **all I/O devices are modeled as files** — persistent storage, networks, terminals, and printers.

---

## Path

A file or directory can be identified by its **path**, which specifies a unique location.

### Absolute Path

Always starts with `/` and specifies the location relative to the root:

```bash
$ pwd
/nfs/stak/users/chaudhrn
```

### Relative Path

Starts at the current working directory (no leading `/`):

| Path | Description |
|------|-------------|
| `foo.txt` | File in the current directory |
| `./foo.txt` | Same, using `.` shortcut |
| `textFiles/bar.txt` | File in a subdirectory |
| `../baz.txt` | File in the parent directory (using `..`) |

---

## Opening and Closing a File

```c
int open(const char *pathname, int flags, mode_t mode);
```

- **`pathname`** — relative or absolute path to the file
- **`flags`** — must include one access mode:
  - `O_RDONLY` — reading only
  - `O_WRONLY` — writing only
  - `O_RDWR` — read and write
  - Plus optional creation/status flags OR-ed together (e.g., `O_CREAT`, `O_TRUNC`, `O_APPEND`)
- **`mode`** — access permissions for newly created files (ignored otherwise)

On success, `open()` returns a **file descriptor** (small non-negative integer). On failure, returns `-1` and sets `errno`.

### Example

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void){
  int file_descriptor;
  char* newFilePath = "./newFile.txt";

  file_descriptor = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, 0600);

  if (file_descriptor == -1){
    printf("open() failed on \"%s\"\n", newFilePath);
    perror("Error");
    exit(1);
  }
  printf("file_descriptor = %d\n", file_descriptor);
  close(file_descriptor);
  return 0;
}
```

### Closing a File

```c
int close(int fd);
```

Closing a file descriptor makes it available for reuse.

---

## Reading and Writing Files

The OS maintains a **file offset** for each open file, which is incremented on reads and writes.

### `read()` System Call

```c
ssize_t read(int fd, void *buf, size_t count);
```

- Reads from `fd` starting at the current offset into `buf`
- Returns the number of bytes read, `0` at EOF, or `-1` on error
- Does **not** allocate memory for the buffer or add a null terminator

### `write()` System Call

```c
ssize_t write(int fd, const void *buf, size_t count);
```

- Writes `count` bytes from `buf` to the file
- Returns the number of bytes written, or `-1` on error
- With `O_APPEND`, the offset is first set to the end of the file

### Example: Write and Read

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void){
  int fd;
  char* newFilePath = "./foo.txt";
  fd = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, 0600);

  if (fd == -1){
    printf("open() failed on \"%s\"\n", newFilePath);
    perror("Error");
    exit(1);
  }

  char message[] = "This be the file!";
  int howMany = write(fd, message, strlen(message) + 1);
  printf("Wrote %d bytes to the file\n", howMany);
  close(fd);

  fd = open(newFilePath, O_RDONLY);
  char* readBuffer = malloc(50 * sizeof(char));
  howMany = read(fd, readBuffer, 50);
  printf("Read %d bytes from the file\n", howMany);
  printf("%s\n", readBuffer);
  free(readBuffer);
  close(fd);
  return 0;
}
```

Output:
```
Wrote 18 bytes to the file
Read 18 bytes from the file
This be the file!
```

---

## `lseek()` System Call

```c
off_t lseek(int fd, off_t offset, int whence);
```

Repositions the file offset:

| `whence` | Description | Example |
|----------|-------------|---------|
| `SEEK_SET` | Set offset to `offset` bytes | `lseek(fd, 16, SEEK_SET)` — move to byte 16 |
| `SEEK_CUR` | Set offset to current + `offset` | `lseek(fd, 4, SEEK_CUR)` — move forward 4 bytes |
| `SEEK_END` | Set offset to end + `offset` | `lseek(fd, -8, SEEK_END)` — 8 bytes from end |

---

## Additional Resources

- [open() — Linux man page](https://man7.org/linux/man-pages/man2/open.2.html)
- Chapter 4 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
- [Files and Directories — OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Arpaci-Dusseau & Arpaci-Dusseau, 2018
