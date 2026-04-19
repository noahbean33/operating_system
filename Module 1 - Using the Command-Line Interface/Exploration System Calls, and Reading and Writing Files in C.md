# Exploration: System Calls, and Reading and Writing Files in C

## Introduction

In the previous exploration we studied the Unix shell, which is an interactive user interface to use services provided by the OS. We will now look at **System Calls**, which provide a programmatic way of using services provided by the OS. We will also begin our learning of the **C programming language** and how we can use C to programmatically access OS services.

---

## System Calls

System calls are the mechanism by which a user program asks the OS to perform services for it. In fact, the shell commands also make system calls to invoke OS services. One popular categorization of system calls divides them into **6 categories**:

### 1. Process Control
- Create process, terminate process
- Load, execute
- Get/set process attributes
- Wait for time, wait event, signal event
- Allocate and free memory

### 2. File Management
- Create file, delete file
- Open, close
- Read, write, reposition
- Get/set file attributes

### 3. Device Management
- Request device, release device
- Read, write, reposition
- Get/set device attributes
- Logically attach or detach devices

### 4. Information Maintenance
- Get/set time or date
- Get/set system data
- Get/set process, file, or device attributes

### 5. Communication
- Create, delete communication connection
- Send, receive messages
- Transfer status information
- Attach or detach remote devices

### 6. Protection
- Get/set file permissions

---

## "Hello World" in C

By convention, the first program when you are learning a language is the "Hello World" program. Here is a "Hello World" program in C:

```c
#include <stdio.h>

int main(void) {
  printf("Hello World\n");
  return 0;
}
```

- In C, as in many other languages, the `main()` function is special: when you run a C program, `main()` is where program execution starts.
- Functions have a **return type** (in this case `int`) and may take **arguments**.
- The body of the function is wrapped in curly braces. In this example it has just 2 lines:
  1. The first line prints `"Hello, world!"` to the terminal.
  2. The second line returns the integer `0`.
- The first line of the program has an `#include` statement to include `stdio.h`, the header for the standard C library for input and output.

---

## "Hello World" Using a System Call

In the "Hello World" example we just studied, our program calls the function `printf()` to write data to the terminal. In Unix, the terminal is modeled as a file (we will study the details later), so in effect `printf()` is writing to a file. Behind the scenes, `printf()` uses the system call `write()` to write to the file.

In our C program, instead of calling `printf()`, we can directly call `write()`:

```c
#include <unistd.h>

// Man page for write system call
// https://www.man7.org/linux/man-pages/man2/write.2.html

int main(void) {
  write(1, "Hello World\n", 12);
  return 0;
}
```

> **Note:** In this exploration, we are not going to look into the details of the `write()` system call. However, over the next few explorations we will have learned every concept employed in this program.

---

## System Calls & Library Functions

As we saw in the previous examples, when we are writing programs in C, many times our programs do **not** directly invoke system calls. Instead, our program may invoke a **C library function**, such as `printf()`, that in turn invokes a system call, such as `write()`, behind the scenes.

As another example, the C standard library provides the function `fopen()` to open a file. Internally, `fopen()` uses the `open()` system call. It is typically easier to code using the C library functions than coding using the underlying system calls.

---

## The GNU C Library (glibc)

**ANSI** is an organization that defines standards. The ANSI C standard specifies a standard library for implementations of the C programming language. Different implementations of Unix have different implementations of the standard C library. The most common of these on Linux is the **GNU C library**, commonly called **glibc**. In this course, we will use glibc.

---

## Additional Resources

- The categorization of system calls given above is from *Operating System Concepts*, by Abraham Silberschatz, Greg Gagne, and Peter B. Galvin, 2018.
- Chapter 4 of *The Linux Programming Interface* has a detailed discussion on file I/O in Linux. We are going to study files in more detail in subsequent modules.
- [The Linux man-pages project](https://www.kernel.org/doc/man-pages/) — Provides extensive documentation on Linux system calls and the C standard library. This documentation is an essential reference for you to use during this course.
