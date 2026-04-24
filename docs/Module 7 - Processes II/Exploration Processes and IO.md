# Exploration: Processes and I/O

## Introduction

This exploration covers the relationship of file descriptors across parent and child processes, and how to redirect standard input and output programmatically.

---

## `fork()`, `exec()` and File Descriptor Inheritance

When a process calls `fork()`, the child **inherits** open file descriptors. Both parent and child share the same **file pointer** — reads/writes from either process move the same offset. This sharing is preserved through `exec()`.

### Example

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
  char *newFilePath = "./newFile.txt";
  printf("PARENT: Opening file %s.\n", newFilePath);
  int fileDescriptor = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, 0760);
  if (fileDescriptor == -1) { printf("open() failed\n"); exit(1); }

  printf("PARENT: Writing 01234 to file.\n");
  write(fileDescriptor, "01234", 5);
  printf("PARENT: FP position: %ld\n", lseek(fileDescriptor, 0, SEEK_CUR));
  fflush(stdout);

  int childExitMethod;
  pid_t spawnPID = fork();

  switch (spawnPID){
  case -1:
    perror("fork() failed\n"); exit(1); break;
  case 0:
    printf("CHILD: started. FP position: %ld\n", lseek(fileDescriptor, 0, SEEK_CUR));
    printf("CHILD: Writing AB to file.\n"); fflush(stdout);
    write(fileDescriptor, "AB", 2);
    printf("CHILD: After write, new FP position: %ld\n", lseek(fileDescriptor, 0, SEEK_CUR));
    fflush(stdout); break;
  default:
    waitpid(spawnPID, &childExitMethod, 0);
    printf("PARENT: child terminated, FP position is: %ld\n", lseek(fileDescriptor, 0, SEEK_CUR));
    fflush(stdout); break;
  }
  return 0;
}
```

To **prevent** sharing, one process can close the file and re-open it to get a new file descriptor.

---

## Redirecting Input and Output

Default file descriptors:
- **stdin:** FD 0 (terminal)
- **stdout:** FD 1 (terminal)
- **stderr:** FD 2 (terminal)

### `dup2()`

```c
#include <unistd.h>

int dup2(int oldfd, int newfd);
```

Duplicates `oldfd` onto `newfd`. After the call, `newfd` points to the same file as `oldfd`. If `newfd` was open, it is automatically closed first.

### Example: Output Redirection

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
  if (argc == 1){ printf("Usage: ./main <filename>\n"); exit(1); }

  int targetFD = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0640);
  if (targetFD == -1) { perror("open()"); exit(1); }

  printf("The file descriptor for targetFD is %d\n", targetFD);

  int result = dup2(targetFD, 1);  // Redirect stdout to targetFD
  if (result == -1) { perror("dup2"); exit(2); }

  // Now printf writes to the file
  printf("All of this is being written to the file using printf()\n");
  return 0;
}
```

### Example: Redirecting Both stdin and stdout

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
  if (argc != 3) { printf("Usage: ./sort_file <input> <output>\n"); exit(1); }

  int sourceFD = open(argv[1], O_RDONLY);
  if (sourceFD == -1) { perror("source open()"); exit(1); }
  printf("File descriptor of input file = %d\n", sourceFD);

  int result = dup2(sourceFD, 0);  // Redirect stdin
  if (result == -1) { perror("source dup2()"); exit(2); }

  int targetFD = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (targetFD == -1) { perror("target open()"); exit(1); }
  printf("File descriptor of output file = %d\n", targetFD);

  result = dup2(targetFD, 1);  // Redirect stdout
  if (result == -1) { perror("target dup2()"); exit(2); }

  execlp("sort", "sort", NULL);
  return 0;
}
```

This is equivalent to: `sort < inputfile.txt > outputfile.txt`

---

## Close File Descriptor on Exec

Use `fcntl()` with `FD_CLOEXEC` to close a file descriptor automatically when `exec()` is called:

```c
#include <fcntl.h>

int fd = open("file", O_RDONLY);
fcntl(fd, F_SETFD, FD_CLOEXEC);
// fd will be closed when this process or a child calls exec()
```

---

## Additional Resources

- Chapters 4, 5, and 18 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
