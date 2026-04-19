# Exploration: Pointers

## Introduction

We now come to **pointers** — one of the most powerful and unique features of C. Pointers hold addresses of variables in memory and allow us to indirectly access these variables. Proper use of pointers facilitates writing very performant programs in C. On the other hand, improper use of pointers is the cause of many bugs and vulnerabilities in C programs.

---

## Pointers in C

A pointer in C is a pointer to a **specific data type**. We can have a pointer to an `int`, `char`, `float`, etc. There are two fundamental operators related to pointers:

| Operator | Description |
|----------|-------------|
| `*p` | Returns the value pointed to by `p` (**dereference**) |
| `&i` | Returns the address of variable `i` (**address-of**) |

A pointer can be declared either way:

```c
type* var;
type *var;
```

### Example

```c
#include <stdio.h>

int main(void) {
  int i;
  int* p;

  // Store 1 in the variable i
  i = 1;

  // Store the address of variable i in variable p
  p = &i;

  // The address of i
  printf("address of i = %p\n", (void*) &i);
  // The value of p (same as the address of i)
  printf("value of p = %p\n", (void*) p);

  // Print the value at the address stored in p (i.e., the value of i)
  printf("*p = %d\n", *p);

  // Increment the value at the address stored in p
  *p = *p + 1;

  printf("*p, i.e., i after increment = %d\n", *p);
  printf("i is now = %d\n", i);

  return 0;
}
```

- `p = &i;` — sets the pointer `p` to the address of `i`
- `*p` — **dereferences** the pointer to access the value of `i`
- `*p = *p + 1;` — modifies the value of `i` through the pointer

> **Aside:** `printf` prints pointer values via `%p` and requires the pointer to be cast to `(void*)`.

---

## Referencing Variables Across Functions

Pointers provide a way for C functions to reference memory allocated outside the function by passing parameters **by reference** rather than **by value**.

### Pass-by-Value (Swap Fails)

```c
#include <stdio.h>

void swap(int val1, int val2){
  printf("val1 = %d val2 = %d\n", val1, val2);
  int temp = val1;
  val1 = val2;
  val2 = temp;
  printf("val1 = %d val2 = %d\n", val1, val2);
}

int main(void) {
  int i = 1;
  int j = 10;
  swap(i, j);
  printf("i = %d j = %d\n", i, j); // i and j are UNCHANGED
  return 0;
}
```

The values of `i` and `j` are **copied** into `val1` and `val2`. Swapping `val1` and `val2` does not affect `i` and `j`.

### Pass-by-Reference (Swap Succeeds)

```c
#include <stdio.h>

void swap(int *val1, int *val2){
  int temp = *val1;
  *val1 = *val2;
  *val2 = temp;
}

int main(void) {
  int i = 1;
  int j = 10;
  swap(&i, &j);
  printf("i = %d j = %d\n", i, j); // i and j ARE swapped
  return 0;
}
```

By passing **pointers** (`&i`, `&j`), the function can dereference them to swap the actual values.

---

## Pointers to Arrays & Pointer Arithmetic

In C, the name of an array is equivalent to a **pointer to its first element**:

```c
int numbers[10];
int* ptr = numbers;      // equivalent to:
int* ptr = &numbers[0];
```

C supports **arithmetic operations on pointers**. Adding an integer to a pointer advances it by that many elements:

| Expression | Equivalent | Equivalent | Description |
|------------|------------|------------|-------------|
| `ptr` | `numbers` | `&numbers[0]` | Address of 1st element |
| `ptr + i` | `numbers + i` | `&numbers[i]` | Address of i-th element |
| `*ptr` | `*numbers` | `numbers[0]` | Value of 1st element |
| `*(ptr + i)` | `*(numbers + i)` | `numbers[i]` | Value of i-th element |

When an array is passed as an argument to a function, it is passed **by reference** (the parameter value is the address of the first element).

### Example: Finding the Maximum

```c
#include <stdio.h>
#define COUNT 5

int findMax(int *ptr){
  int max = *ptr;
  for(int i = 1; i < COUNT; i++){
    if(*(ptr + i) > max){
      max = *(ptr + i);
    }
  }
  return max;
}

int main(void) {
  int numbers[COUNT];
  for(int i = 0; i < COUNT; i++){
    printf("Enter an integer: ");
    scanf("%d", &numbers[i]);
  }
  printf("The max number you entered was %d\n", findMax(numbers));
  return 0;
}
```

> **Behind the scenes:** C automatically computes the address of the element based on the size of the data type. `ptr + 1` equals the value of `ptr` plus `sizeof(int)`.

---

## Exercise

The following program defines a function `stringLength` that returns the length of a string. The code is incomplete — add a line to advance the pointer:

```c
#include <stdio.h>

int stringLength(char *ptr){
  int length = 0;
  while((*ptr) != '\0'){
    length++;
    // Fix: Add a line to advance the pointer
  }
  return length;
}

int main(void) {
  char aString[] = "Hello!";
  printf("\"Hello!\" is %d characters long\n", stringLength(aString));
  return 0;
}
```

---

## Additional Resources

- Chapter 11 of *Modern C* discusses pointers.
