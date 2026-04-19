# Exploration: Arrays & Structures

## Introduction

**Arrays** allow us to store a group of elements of the same data type together in memory, while **structures** allow us to group elements of different types.

---

## Arrays

We can define arrays of any of the basic data types (in addition to other user-defined types). You can declare an array by specifying the name, the data type of its elements, and the number of elements it can store.

```c
#include <stdio.h>

int main(void) {
  int count = 5;
  int numbers[count];
  for(int i = 0; i < count; i++){
    printf("Enter an integer: ");
    scanf("%d", &numbers[i]);
  }

  printf("You entered the following numbers: ");
  for(int i = 0; i < count; i++){
    printf("%d ", numbers[i]);
  }

  return 0;
}
```

- Individual elements of an array are read and written to just like an ordinary variable of that type.
- Elements are referenced via a **0-based index** whose value goes up to `size - 1`.
- The elements of an array are allocated **together in memory**.

---

## Strings — Arrays of Characters

In C, a string is an **array of characters terminated with the null character** `\0`.

```c
#include <stdio.h>

int main(void) {
  // message is an array with 4 characters
  char message[] = "Hi!";

  // Allocate 3 chars: 2 for the state and 1 for the null terminator
  char state[3];

  printf("Enter the 2-letter code for the state you live in: ");
  scanf("%s", state);
  printf("%s you live in %s\n", message, state);
}
```

The array `message` is initialized to `"Hi!"` which is **4 characters** long: `'H'`, `'i'`, `'!'`, `'\0'`.

Equivalent declarations:

```c
char message[] = {"Hi!"};
char message[4] = {"Hi!"};
char message[] = {'H', 'i', '!', '\0'};
```

- Both `printf` and `scanf` work with strings using the `%s` format character.
- `scanf` reads until whitespace is encountered and stores the value terminated with a null character.
- Note that we **don't** write `&` before `state` in `scanf` (explained in the Pointers exploration).
- The size of the input must not exceed the size of the array.

---

## Passing Arguments from the Command Line

All the programs we have written so far were executed by running `./main`. If the program required input, we used `scanf`. However, it is also possible to **pass arguments from the command line**:

```c
int main(int argc, const char * argv[])
```

- `argc` — the number of space-delimited strings entered on the command line
- `argv` — an array holding each of the strings

**Examples:**

| Command | `argc` | `argv` contents |
|---------|--------|-----------------|
| `./main` | 1 | `"./main"` |
| `./main file.txt 8` | 3 | `"./main"`, `"file.txt"`, `"8"` |

The C standard library provides [`atoi`](https://man7.org/linux/man-pages/man3/atoi.3.html) (ASCII to integer) to convert string arguments to integers.

### Example: Factorial from Command Line

```c
#include <stdio.h>
#include <stdlib.h> // for atoi

/*
 * Compute the factorial of x
 */
long factorial(int x){
  long result = 1;
  for (int i = 1; i <= x; i++){
    result *= i;
  }
  return result;
}

/* Print the factorial of a number */
int main(int argc, const char * argv[]) {
  // Verify that the command line contains two strings
  if(argc != 2){
    printf("Please specify an integer value between 1 and 20\n");
    printf("Usage: ./main intVal\n");
    return 1;
  }
  // Get the second string and convert it to an integer
  int i = atoi(argv[1]);
  if(i < 1 || i > 20){
    printf("The number you entered is outside the allowed values\n");
    return 1;
  }
  printf("The factorial of %d is %ld\n", i, factorial(i));
  return 0;
}
```

Run with: `./main 10`

---

## Structures

**Structures** provide a way to group members together which can have different data types. Structures in C are somewhat like a class in Java or C# but **without the methods**. Members of a structure are accessed using a **dot** (`.`) after the variable name.

### Example

```c
#include <stdio.h>

int main(void) {
    struct student {
        char* name;
        int studentId;
        char* major;
    };

    struct student johnDoe;
    johnDoe.name = "John Doe";
    johnDoe.studentId = 344;
    johnDoe.major = "Computer Science";
    printf("%s has id %d and major %s\n", johnDoe.name, johnDoe.studentId, johnDoe.major);

    // We can initialize a struct by providing the elements in {}
    struct student janeDoe = {
        "Jane Doe", 493, "Computer Science"
    };
    printf("%s has id %d and major %s\n", janeDoe.name, janeDoe.studentId, janeDoe.major);
    return 0;
}
```

### Linked Lists with Structures

Linked lists in C are frequently created by defining a structure that includes a **pointer to its own type**:

```c
struct student {
  char* name;
  int studentId;
  char* major;
  struct student *next;
};
```

---

## Additional Resources

- [C for Python Programmers](http://www.cburch.com/books/cpy/) — Carl Burch, Hendrix College, 2011
- [The C Book](https://publications.gbdirect.co.uk/c_book/) — Mike Banahan, Declan Brady, and Mark Doran, 1991
