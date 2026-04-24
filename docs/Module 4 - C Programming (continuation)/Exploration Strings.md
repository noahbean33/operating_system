# Exploration: Strings

## Introduction

Strings in C are simply an **array of characters terminated by the null character** (`\0`). It is not a basic data type in C, unlike many newer languages such as Java or C++. However, C includes many functions for working with strings. We will first look at various ways of declaring strings and their implications on memory allocation, then study some basic C library functions for manipulating strings.

---

## Declaring Strings & Memory Allocation

Two different ways to declare the same 10-character string (including the null terminator):

```c
char* myString = "my string";
char myString[] = "my string";
```

While both create a string with the same characters, there are **important differences** in how memory is allocated.

### String Allocation in the Data Segment

```c
char* myString = "my string";
```

```c
#include <stdio.h>

int main(void) {
  char* myString = "my string";
  printf("myString is %s\n", myString);

  // We try to modify the second character to 'e'
  myString[2] = 'e';  // CRASH!
  printf("myString is %s\n", myString);

  return 0;
}
```

**Why it crashes:**
- `myString` is a pointer allocated on the **stack**
- The string literal `"my string"` is stored in the **read-only Data Segment**
- `myString` stores the address of that read-only memory
- Attempting to modify read-only memory causes a crash

### String Allocation in the Stack Segment

```c
char myString[] = "my string";
char myString[10] = "my string";
```

Both declarations allocate **10 bytes on the stack**. Stack memory is editable:

```c
#include <stdio.h>

int main(void) {
  char myString[10] = "my string";
  printf("myString is %s\n", myString);

  myString[2] = 'e';  // Works fine!
  printf("myString is %s\n", myString);

  return 0;
}
```

---

## Basic String Functions

The header file `string.h` contains declarations for many string library functions.

### Example Program

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    char* myString1 = "my string1";
    char myString2[11] = "my string2";

    // strlen returns the length excluding the null character
    printf("\"%s\" is %lu characters long\n", myString1, strlen(myString1));

    // Compare myString1 and myString2
    if(strcmp(myString1, myString2)){
        printf("myString1 and myString2 are not equal\n");
    } else {
        printf("myString1 and myString2 are equal\n");
    }

    // Compare first 9 characters
    if(strncmp(myString1, myString2, 9)){
        printf("First 9 characters are not equal\n");
    } else {
        printf("First 9 characters are equal\n");
    }

    // Copy myString1 into myString3
    char* myString3 = calloc(strlen(myString1) + 1, sizeof(char));
    strcpy(myString3, myString1);
    printf("myString3 is \"%s\"\n", myString3);

    // Concatenate myString1 with myString2
    char* myString4 = calloc(strlen(myString1) + strlen(myString2) + 1, sizeof(char));
    strcpy(myString4, myString1);
    strcat(myString4, myString2);
    printf("myString4 is \"%s\"\n", myString4);
    return 0;
}
```

### `strcmp` & `strncmp`

```c
int strcmp(const char *s1, const char *s2);
```

Compares strings `s1` and `s2`. Returns **0** if equal, non-zero otherwise.

> **Important:** Do not use `==` to compare strings — that compares pointer values, not string contents.

`strncmp` takes a 3rd argument `n` and compares only the first `n` characters.

### `strlen`

```c
size_t strlen(const char *s);
```

Returns the length of the string, **not** including the null terminator.

### `strcpy`

```c
char *strcpy(char *dest, const char *src);
```

Copies `src` to `dest` (including the null character). The destination buffer must be large enough, or **buffer overflow** occurs. `strncpy` copies at most `n` characters.

### `strcat`

```c
char *strcat(char *dest, const char *src);
```

Appends `src` to the end of `dest`. Overwrites the null character in `dest` and adds a new null character at the end. The destination must have enough space. `strncat` appends at most `n` characters.

### `strdup`

```c
char *strdup(const char *s);
```

Creates a **duplicate** of the string `s` using `malloc` and returns a pointer to the new string.

---

## Additional Resources

- [string.h — Open Group C Specification](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/string.h.html)
- [Arrays as Strings — GNU C Reference](https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html)
