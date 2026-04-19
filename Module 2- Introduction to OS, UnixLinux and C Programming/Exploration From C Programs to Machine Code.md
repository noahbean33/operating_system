# Exploration: From C Programs to Machine Code

## Introduction

If your previous programming experience has been with a language like Python, you may have noticed a difference in how you execute your Python programs vs. your C programs. **C is a compiled language.** Before a C program can be executed, it must first be transformed into **machine code** — the binary version of assembly instructions. This process can be broken into a few steps.

---

## Pre-processing

The **C pre-processor** is responsible for analyzing and processing special statements identified with a `#`. We have already seen one example: the `#include` directive, which is used to include header files in a program. Header files typically contain the descriptions of functions and variables needed by the program.

Another type of special statement processed by the C pre-processor is called a **macro**. Macros are defined with `#define`:

```c
#define PI 3.14
```

If a program includes the above macro, the C pre-processor will replace all instances of `PI` in the program with the value `3.14`. A common use of macros is to define a symbolic constant once and use it in multiple places in the program.

> **Note:** There is no semicolon at the end of the `#define` directive.

Additional directives that the pre-processor handles include `#ifdef`, `#endif`, `#else`, and `#ifndef`, which are used for **conditional compilation**.

---

## Compilation

The **C compiler** is responsible for:

1. Parsing your code
2. Checking it for errors
3. Generating **assembly language** code

The compiler then calls the **assembler**, which converts the assembly code into **machine binary code**. A file with machine binary code is commonly referred to as an **object file** and has the extension `.o`.

We will use the **gcc** compiler in this course. Some of the most useful gcc options:

| Flag | Description |
|------|-------------|
| `-g` | Compile with debugging info for GDB |
| `-c` | Compile only, without linking |
| `-S` | Generate assembly code |
| `-O3` | Optimize as much as possible |
| `-o` | Specify the name of the output file |
| `-Wall` | Turn on all warnings |
| `-l<library>` | Add support for a library when linking (e.g., `-lpthread`) |
| `-std=c99` | Use the C99 standard |
| `-std=gnu99` | Use the C99 standard with GNU extensions |

---

## Compiler Warnings

If the compiler finds an **error** in your program, compilation will fail. For example, if you misspell the name of the function `main` as `mian`, compilation will fail and the compiler will report an error.

In addition to errors, the compiler can also report **warnings** — diagnostic messages indicating possible problems with your program. By default, warnings do not cause compilation to fail and your program may even run correctly in the presence of warnings. However, warnings indicate possible issues that can cause program execution to fail or have **unspecified behavior**.

**Example:** In the following program, change `%d` to `%f` in line 20. If you compile, you will see a warning but the program will successfully compile. However, when you run the program, the behavior is incorrect.

```c
#include <stdio.h>

/**
 * Ask the user for two positive integers.
 * Print all the multiples of the first integer between 1 and the second integer
 */
int main(void) {
  int boundary;
  int denominator;

  printf("Enter a positive integer whose multiples you want printed: ");
  scanf("%d", &denominator);
  printf("Enter another positive integer up to which you want to see multiples printed: ");
  scanf("%d", &boundary);
  printf("Here are all the multiples of %d between 1 & %d\n", denominator, boundary);
  for(int i = 1; i <= boundary; i++){
    if(i % denominator == 0){
      printf("%d ", i);
    }
  }
  printf("\n");
  return 0;
}
```

### Options Related to Warnings

gcc supports many options related to warnings. The most commonly used is `-Wall`, which despite its name does **not** cover all possible warnings. See the [GCC Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) documentation for more information.

> **Tip:** Always review and fix all warnings reported by the compiler.

---

## Linking

Virtually all C programs depend on **multiple files**. Even a simple `hello_world.c` program needs to include `stdio.h` to use the `printf` function. `stdio.h` includes the *description* of `printf`, but does not include its code.

The **linker** (or link editor) is the part of the compilation chain that stitches or links together the various object files and creates one executable file.

In addition to linking object files, the linker will also link **library archives**, which are collections of object files (`.o`) gathered into a single large file. Library archive files have indexes that make accessing them fast. This is especially useful when the object files in the library seldom change.

The **standard C library** (`libc`) contains the object code for standard C functions such as `printf` and is automatically linked in by gcc without you needing to specify it.

---

## Additional Resources

- [Discussion of Macros](https://gcc.gnu.org/onlinedocs/cpp/Macros.html)
- [Standard Library libc on Linux](https://www.gnu.org/software/libc/)
- [GCC Version 13 Manual](https://gcc.gnu.org/onlinedocs/gcc-13.1.0/gcc/)
- [GCC Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
- [Clang Compiler User Manual](https://clang.llvm.org/docs/UsersManual.html)
