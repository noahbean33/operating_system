# Exploration: Variables & Data Types, Input & Output in C

## Introduction

Let us start with a simple C program that asks the user for two integers and then prints all the multiples of the first integer between 1 and the second integer. We will start writing the program in this exploration and finish it in a later exploration.

```c
#include <stdio.h>

/**
 * Ask the user for two positive integers.
 * Print all the multiples of the first integer between 1 and the second integer
 */
int main(void) {
    int denominator;
    int boundary;

    printf("Enter a positive integer whose multiples you want printed: ");
    scanf("%d", &denominator);
    printf("Enter another positive integer up to which you want to see multiples printed: ");
    scanf("%d", &boundary);
    printf("You entered %d for denominator and %d for boundary\n", denominator, boundary);
    return 0;
}
```

---

## Main Entry Point

Execution begins with the body of the `main` function. In our program, `main` is defined such that it doesn't take any arguments and returns an `int`. When the program runs, it returns the value `0` to the OS. On most OSs, a return value of **0** indicates successful execution, while a **non-zero** value indicates an error.

---

## Variables & Data Types

In our program, we declare 2 variables:

```c
int denominator;
int boundary;
```

In C, the data type of each variable must be **explicitly declared at compile-time**. This is in contrast with languages such as Python, where the type of a variable does not need to be explicitly specified and can be inferred at run-time.

A variable of type `int` can only store an integer value. In addition to `int`, C provides three additional basic data types:

| Type | Description |
|------|-------------|
| `float` | Floating-point numbers (numbers containing decimal points) |
| `double` | Similar to `float`, but with **double the precision** |
| `char` | A single character, specified within single quotes (e.g., `'A'`, `'1'`, `','`) |

> **Note:** `string` is **not** supported as a basic type in C!

---

## `#include` Directive

In our program, we call two functions multiple times: `printf()` and `scanf()`. These functions are not defined in our program. In C, we can use functions defined elsewhere by including the **header files** that contain the descriptions of these functions:

```c
#include <stdio.h>
```

As our C program is transformed into an executable, the contents of this header file are copied into our program. Header files typically contain descriptions of functions and definitions of variables.

---

## Output Using `printf()`

The header file `stdio.h` contains the description of many functions related to input and output. Let us look at `printf()`:

```c
printf("Enter a positive integer whose multiples you want printed: ");
printf("Enter another positive integer up to which you want to see multiples printed: ");
printf("You entered %d for denominator and %d for boundary\n", denominator, boundary);
```

The first two calls are simple — the function is called with **one argument**, which is a string.

### Quick Introduction to Strings in C

A string in C is an **array of characters terminated by the null character** (`\0`). Unlike a single character, a string is enclosed in **double quotes**:

```c
char oneChar = 'A';           // A single character
char stringWithCharA[] = "A"; // An array of characters: {'A', '\0'}
```

- `oneChar` is of type `char`
- `stringWithCharA` is an **array of characters** with 2 elements: `'A'` and `'\0'`

### Format Specifiers in `printf()`

The third call to `printf()` uses **format specifiers** (`%d`) as placeholders for variable values:

```c
printf("You entered %d for denominator and %d for boundary\n", denominator, boundary);
```

| Parameter | Data Type |
|-----------|-----------|
| `%d` or `%i` | Decimal integer |
| `%x` | Hexadecimal integer |
| `%f` | Floating-point number |
| `%c` | ASCII character |

---

## Exercise

Run the program and input a number greater than 15 when asked for the second integer. Observe the output. Now change `%d` to `%x` in the 3rd `printf()` statement and run the program again with the same numbers. Observe the output and explain the reason for any differences.

---

## Input Using `scanf()`

We used `scanf()` two times in our program to get an integer value from the user:

```c
scanf("%d", &denominator);
scanf("%d", &boundary);
```

- The **first argument** is a string containing formatting instructions to read input. Just as `%d` in `printf()` means *print a decimal integer*, `%d` in `scanf()` means *read a decimal integer*.
- The **second argument** contains the name of the variable to which `scanf()` assigns the value (for now, ignore the `&` — we will discuss it later).

### Format Specifiers in `scanf()`

| Parameter | Data Type |
|-----------|-----------|
| `%d` | Decimal integer |
| `%i` | Octal (if starts with `0`), hex (if starts with `0x`), otherwise decimal |
| `%x` | Hexadecimal integer |
| `%f` | Floating-point number |
| `%c` | ASCII character |

---

## Additional Resources

- **C Programming Crash Course** — An instructional YouTube video introducing C
- *Modern C* by Jens Gustedt (Manning, 2019) — See Chapter 14.2 for a discussion of `scanf()`
- [C for Python Programmers](http://www.cburch.com/books/cpy/) — Carl Burch, Hendrix College, 2011
