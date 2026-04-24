# Exploration: Pipes & FIFO

## Introduction

Pipes and FIFOs provide simple mechanisms for **inter-process communication** by creating data channels between processes.

---

## Pipes

Pipes are used for IPC between two processes that share a **common ancestor** (via `fork()`). A pipe is a **unidirectional** data channel connecting a write-only FD in one process to a read-only FD in another.

### Creating a Pipe

1. Parent calls `pipe()` → creates two file descriptors
2. Parent calls `fork()` (and possibly `exec()`)
3. Both parent and child have both FDs
4. Writer writes to `pipefd[1]`; reader reads from `pipefd[0]`

### `pipe()` Function

```c
#include <unistd.h>

int pipe(int pipefd[2]);
```

- `pipefd[0]` — **read** end
- `pipefd[1]` — **write** end
- Returns 0 on success, -1 on error

### Example: Child Sends Data to Parent

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
  char readBuffer[10];
  char completeMessage[512] = {0};
  int r, pipeFDs[2];

  if (pipe(pipeFDs) == -1) { perror("pipe() failed"); exit(1); }

  pid_t spawnpid = fork();
  switch (spawnpid){
    case 0:
      // Child: close read end, write to pipe
      close(pipeFDs[0]);
      write(pipeFDs[1], "CHILD: Hi parent!@@", 19);
      exit(0);
      break;
    default:
      // Parent: close write end, read from pipe
      close(pipeFDs[1]);
      while (strstr(completeMessage, "@@") == NULL){
        memset(readBuffer, '\0', sizeof(readBuffer));
        r = read(pipeFDs[0], readBuffer, sizeof(readBuffer) - 1);
        strcat(completeMessage, readBuffer);
        printf("PARENT: Chunk received: \"%s\"\n", readBuffer);
        printf("PARENT: Total received: \"%s\"\n", completeMessage);
        if (r == -1) { printf("r == -1\n"); return EXIT_FAILURE; }
        if (r == 0)  { printf("r == 0\n");  return EXIT_FAILURE; }
      }
      char* terminalLoc = strstr(completeMessage, "@@");
      *terminalLoc = '\0';
      printf("PARENT: Complete string: \"%s\"\n", completeMessage);
      break;
  }
  return EXIT_SUCCESS;
}
```

### Pipe Size and Blocking

- Linux pipes are typically **65,536 bytes** (64 KB)
- `write()` **blocks** if the pipe is full
- `read()` **blocks** if no data is available

### The `read()` Function

```c
ssize_t read(int fd, void *buf, size_t count);
```

Reads up to `count` bytes. Returns the number of bytes read (may be less than `count`), 0 on EOF, -1 on error.

### The `strstr()` Function

```c
char *strstr(const char *haystack, const char *needle);
```

Finds first occurrence of `needle` in `haystack`. Returns pointer to match or `NULL`.

### Closing Pipes

- **Reader closes read end** → writer's `write()` returns -1 and process receives `SIGPIPE`
- **Writer closes write end** → reader reads remaining data, then `read()` returns 0 (EOF)

---

## FIFO (Named Pipes)

FIFOs are similar to pipes but have a **name** in the filesystem, so **unrelated processes** can use them. Can have multiple readers and writers.

### `mkfifo()` Function

```c
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
```

Creates a named FIFO. Returns 0 on success, -1 on error. When a process calls `open()` on a FIFO, it **blocks** until another process also opens it.

### Example: Reader Program

```c
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
  char readBuffer[10];
  char completeMessage[512] = {0};
  int bytesRead;
  char* FIFOfilename = "myNewFifo";

  mkfifo(FIFOfilename, 0640);

  int fd = open(FIFOfilename, O_RDONLY);
  if (fd == -1) { perror("Reader: open()"); exit(1); }

  while (strstr(completeMessage, "@@") == NULL){
    memset(readBuffer, '\0', sizeof(readBuffer));
    bytesRead = read(fd, readBuffer, sizeof(readBuffer) - 1);
    strcat(completeMessage, readBuffer);
    printf("Reader: Chunk received: \"%s\"\n", readBuffer);
    if (bytesRead == -1) { printf("bytesRead == -1\n"); break; }
    if (bytesRead == 0)  { printf("bytesRead == 0\n");  break; }
  }
  char* terminalLoc = strstr(completeMessage, "@@");
  *terminalLoc = '\0';
  printf("Reader: Complete string: \"%s\"\n", completeMessage);
  remove(FIFOfilename);
}
```

### Example: Writer Program

```c
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){
  char stringToWrite[21] = "WRITER: Hi reader!@@";
  char* FIFOfilename = "myNewFifo";

  sleep(10);  // Wait for reader to create the FIFO

  int fd = open(FIFOfilename, O_WRONLY);
  if (fd == -1) { perror("Writer: open()"); exit(1); }

  write(fd, stringToWrite, strlen(stringToWrite));
  exit(0);
}
```

### Creating FIFOs from the Shell

```bash
$ mkfifo my_fifo
$ ls -l my_fifo
prw-rw----. 1 user group 0 May 25 06:13 my_fifo
```

The `p` in the first column indicates a FIFO (pipe) file.

---

## Additional Resources

- Chapter 44 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
