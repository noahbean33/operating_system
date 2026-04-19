# Exploration: Parsing Strings

## Introduction

The C string library has two functions, `strtok` and `strtok_r`, that are widely used for parsing strings. In this exploration, we will study these functions.

---

## `strtok`

```c
char *strtok(char *str, const char *delim);
```

`strtok` breaks a string into a sequence of nonempty tokens using the characters provided in `delim` as delimiters. The token returned is a **null-terminated string** that doesn't include the delimiting character. If no more tokens are found, `strtok` returns `NULL`. A sequence of more than one contiguous delimiter is considered to terminate one token.

### Example

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    char input[18] = "This.is my/string";
    // In the 1st call, we specify the string to tokenize.
    // Delimiters are space, dot and /
    char* token = strtok(input, " ./");

    // 1st token will be "This"
    printf("1st token = %s\n", token);
    // After the first call, the first argument is NULL
    token = strtok(NULL, " ./");
    // 2nd token will be "is"
    printf("2nd token = %s\n", token);

    // We change the delimiters so that space is the only delimiter
    token = strtok(NULL, " ");
    // 3rd token will be "my/string"
    printf("3rd token = %s\n", token);

    // We are at the end of the string
    token = strtok(NULL, " ");
    // strtok will return a NULL value
    printf("No more tokens. We get back %s\n", token);
    return 0;
}
```

**Key points:**
- The string to be parsed is provided as the first argument in the **first** call
- In subsequent calls, the first argument should be `NULL`
- If a non-null string is passed in a subsequent call, `strtok` starts parsing that new string
- The delimiters can be different in successive calls

---

## `strtok_r`

```c
char *strtok_r(char *str, const char *delim, char **saveptr);
```

`strtok_r` is a **reentrant** version of `strtok`. With `strtok_r`, we can parse **different strings concurrently**, whereas with `strtok` we cannot. The 3rd argument `saveptr` is used to maintain context between successive calls.

> **Tip:** Using `strtok_r` instead of `strtok` is typically less error-prone.

### Example

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
  char str1[18] = "This.is my/string";
  char str2[24] = "A very different string";
  char* savePtr1;
  char* savePtr2;

  char* token = strtok_r(str1, " ./", &savePtr1);
  printf("1st token in str1 = %s\n", token);   // "This"

  token = strtok_r(str2, " ./", &savePtr2);
  printf("1st token in str2 = %s\n", token);    // "A"

  token = strtok_r(NULL, " ./", &savePtr1);
  printf("2nd token in str1 = %s\n", token);    // "is"

  token = strtok_r(NULL, " ./", &savePtr2);
  printf("2nd token in str2 = %s\n", token);    // "very"

  return 0;
}
```

---

## Things to Watch For

### Don't Pass Read-Only Strings

When parsing, `strtok` and `strtok_r` **overwrite the delimiter character** at the end of the current token with the null character. This means they **modify** the input string. If you pass a string allocated in the **read-only Data Segment**, the program will crash. A workaround is to make a copy of the string first.

### Tokens Are Pointers into the Original String

`strtok`/`strtok_r` do **not allocate memory** for the token they return. The returned token is just a pointer within the string being parsed. If you want to preserve the token values after the original string goes out of scope or is modified, you must **allocate memory and copy** the values.

### Example: Pointer Aliasing Issue

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    char student[25] = "John Doe, CS";
    char* savePtr;
    char* token;
    char* name;

    token = strtok_r(student, ",", &savePtr);
    name = token;  // name points to the SAME memory as student

    if(name == student){
        printf("name and student have the same address with value %p\n", (void*) student);
    }

    // Copy a new string into student
    strcpy(student, "Jane Smith, Math");
    // name now points to the new string!
    printf("%s\n", name);  // Prints "Jane Smith, Math"

    return 0;
}
```

### Exercise

In the above example, change `name = token;` so that the program prints the original `"John Doe"` even after `student` is overwritten. (Hint: allocate memory and use `strcpy`.)

---

## Additional Resources

- [strtok and strtok_r — Linux man pages](https://man7.org/linux/man-pages/man3/strtok.3.html)
- [How to use strtok and strtok_r](https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/)
