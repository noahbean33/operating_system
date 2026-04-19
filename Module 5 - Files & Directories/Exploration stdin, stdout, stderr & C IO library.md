# Exploration: stdin, stdout, stderr & C I/O Library

## Introduction

**Stream** is another abstraction for I/O in Unix. Streams provide **buffered I/O** — they contain a buffer and a reference to a file. Many system calls operate at the file level, but the C standard I/O library functions provide support for streams at a higher level of abstraction.

---

## stdin, stdout, stderr

Each process in Unix is preconnected with **three open files**:

### Standard Input (`stdin`)
- File descriptor: **0** (or `STDIN_FILENO` from `unistd.h`)
- By default associated with the **keyboard**

### Standard Output (`stdout`)
- File descriptor: **1** (or `STDOUT_FILENO`)
- By default associated with the **terminal**

### Standard Error (`stderr`)
- File descriptor: **2** (or `STDERR_FILENO`)
- By default associated with the **terminal**

---

## Redirecting Standard Input and Output

From the bash shell:
- **Input redirection:** `<`
- **Output redirection:** `>`

```bash
$ echo 'this is a line'
this is a line

$ echo 'this is a line' > echo.txt
$ cat echo.txt
this is a line
```

---

## I/O Functions in C Standard Library

Instead of system calls, most developers use functions from `stdio.h`. A **stream** is represented by `FILE*`.

### Opening & Closing a Stream

| Function | Signature | Notes |
|----------|-----------|-------|
| `fopen()` | `FILE *fopen(const char *pathname, const char *mode)` | Opens a file; modes: `"r"` read, `"w"` write, `"a"` append |
| `fclose()` | `int fclose(FILE *stream)` | Flushes and closes the stream |

### Reading from a Stream

| Function | Signature | Notes |
|----------|-----------|-------|
| `fgetc()` | `int fgetc(FILE *stream)` | Reads next character; returns `EOF` on end/error |
| `fgets()` | `char *fgets(char *s, int size, FILE *stream)` | Reads at most `size-1` chars; stops at EOF or newline; null-terminates |
| `getline()` | `ssize_t getline(char **lineptr, size_t *n, FILE *stream)` | Reads entire line; allocates buffer if `*lineptr` is `NULL` |
| `fscanf()` | `int fscanf(FILE *stream, const char *format, ...)` | Like `scanf`, but from a specified stream |

### Writing to a Stream

| Function | Signature | Notes |
|----------|-----------|-------|
| `fputc()` | `int fputc(int c, FILE *stream)` | Writes one character |
| `fputs()` | `int fputs(const char *s, FILE *stream)` | Writes string without null terminator |
| `fprintf()` | `int fprintf(FILE *stream, const char *format, ...)` | Like `printf`, but to a specified stream |
| `fread()` | `size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)` | Binary block read |
| `fwrite()` | `size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)` | Binary block write |

### Repositioning a Stream

| Function | Signature | Notes |
|----------|-----------|-------|
| `fseek()` | `int fseek(FILE *stream, long offset, int whence)` | stdio version of `lseek()` |

### Checking and Resetting Stream Status

| Function | Signature | Notes |
|----------|-----------|-------|
| `feof()` | `int feof(FILE *stream)` | Test end-of-file indicator |
| `fileno()` | `int fileno(FILE *stream)` | Get the file descriptor |
| `ferror()` | `int ferror(FILE *stream)` | Test error indicator |
| `clearerr()` | `void clearerr(FILE *stream)` | Clear end-of-file and error indicators |

---

## Example: Processing a File Line by Line

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void processFile(char* inputFilePath, char* outputFileName){
    char *currLine = NULL;
    size_t len = 0;
    int lineNum = 1;

    FILE *inputFile = fopen(inputFilePath, "r");

    char *outputFilePath = malloc(9 + 1 + strlen(outputFileName) + 1);
    sprintf(outputFilePath, "%s/%s", "outputDir", outputFileName);
    FILE *outputFile = fopen(outputFilePath, "w");

    while(getline(&currLine, &len, inputFile) != -1){
        fprintf(outputFile, "%d %s", lineNum, currLine);
        lineNum++;
    }

    free(currLine);
    free(outputFilePath);
    fclose(inputFile);
    fclose(outputFile);
}

int main(int argc, char **argv){
    if (argc < 3){
        printf("You must provide the names of the input file and the output file\n");
        return EXIT_FAILURE;
    }
    processFile(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
```

**Key points:**
- `fopen()` with `"r"` for reading, `"w"` for writing (creates or truncates), `"a"` for appending
- `getline()` allocates a buffer if passed `NULL`; always null-terminates; returns `-1` at EOF
- `sprintf()` writes formatted output to a string
- `fprintf()` writes formatted output to a file stream

---

## `fscanf()` vs. `fgets()` / `getline()`

The Linux man page notes: *"It is very difficult to use these functions correctly, and it is preferable to read entire lines with `fgets()` or `getline()` and parse them later."*

---

## Additional Resources

- [C stdio Functions](https://en.wikipedia.org/wiki/C_file_input/output)
