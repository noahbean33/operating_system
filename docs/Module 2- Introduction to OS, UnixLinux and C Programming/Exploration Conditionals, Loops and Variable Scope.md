# Exploration: Conditionals, Loops and Variable Scope

## Making Decisions Using Conditionals

C supports various ways to make decisions using conditionals.

### `if` Statements

C's `if` statements are very similar to if statements in other programming languages. Note the parentheses around the Boolean expression and the use of braces for the code blocks. We have the basic `if` statement which will execute if the provided expression evaluates to true. We also have the familiar `if-then-else` statement, where we have additional `else if` branches with expressions and optionally an `else` branch without an expression which will be executed if none of the expressions provided to the other branches evaluate to true.

```c
if (expr) {
    //...
} else if (expr2) {
    // ...
} else {
    // ...
}
```

The parentheses around the Boolean expression are **mandatory**. But if a code block contains only one statement, we can omit the braces.

---

### `switch` Statements

In many programs, we end up using an `if-then-else` statement that compares the same variable to different values.

**Example:** Consider the following program in which an integer value entered by the user is compared to many different values:

```c
#include <stdio.h>

int main(void) {
  printf("Hi! What is your favorite day of the week? Enter it as an integer "
         "with Sunday as 1, Monday as 2, ... and Saturday as 7\n");

  int day;
  scanf("%d", &day);
  printf("Your favorite day of the week is: ");

  if(day == 1){
    printf("Sunday");
  } else if(day == 2){
    printf("Monday");
  } else if(day == 3){
    printf("Tuesday");
  } else if(day == 4){
    printf("Wednesday");
  } else if(day == 5){
    printf("Thursday");
  } else if(day == 6){
    printf("Friday");
  } else {
    printf("Saturday");
  }
  printf("\n");
  return 0;
}
```

Such `if-then-else` statements are frequent enough that C provides a `switch` statement construct to code them. Here is the same program using the `switch` statement:

```c
#include <stdio.h>

int main(void) {
  printf("Hi! What is your favorite day of the week? Enter it as an integer "
         "with Sunday as 1, Monday as 2, ... and Saturday as 7\n");

  int day;
  scanf("%d", &day);
  printf("Your favorite day of the week is: ");

  switch(day){
  case 1:
    printf("Sunday");
    break;
  case 2:
    printf("Monday");
    break;
  case 3:
    printf("Tuesday");
    break;
  case 4:
    printf("Wednesday");
    break;
  case 5:
    printf("Thursday");
    break;
  case 6:
    printf("Friday");
    break;
  default:
    printf("Saturday");
  }
  printf("\n");
  return 0;
}
```

- The expression is evaluated **once**.
- The matching `case` branch is executed.
- The code to be executed for the branch ends with a `break` statement.
- If none of the `case` branches match, the `default` branch will be executed.

---

### Conditional (Ternary) Operator

A conditional operator is a very succinct way of coding a simple decision. The general format is:

```c
condition ? expression1 : expression2
```

- If the condition is **true**, `expression1` is evaluated and its value is returned.
- Otherwise, `expression2` is evaluated and its value is returned.

A common usage is to set the value of a variable to one of two expressions based on a condition:

```c
#include <stdio.h>

int main(void) {
  int x, y;
  int maxInt;

  printf("Enter an integer: ");
  scanf("%d", &x);
  printf("Enter a different integer: ");
  scanf("%d", &y);

  maxInt = (x > y) ? x : y;
  printf("%d is the bigger of %d and %d\n", maxInt, x, y);

  return 0;
}
```

---

## Loops

### Looping with `for`

We now complete the program from a previous exploration that asks the user for two integers and then prints multiples of the first integer between 1 and the second integer.

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

The `for` statement has the following general form:

```c
for (initialization_statement; loop_condition; repeating_statement) {
    // Statement(s) to execute
}
```

1. The **initialization statement** is executed once.
2. The **loop condition** is evaluated for each execution of the loop (including the first). If it evaluates to `true`, the body is executed.
3. After the body has been executed, the **repeating statement** is executed.
4. The loop condition is evaluated again. The `for` loop continues until the condition evaluates to `false`.

> **Note:** Declaring the variable `i` in the initialization statement (e.g., `for(int i = 1; ...)`) is an enhancement added by the **C99** standard. When compiling with gcc, make sure to use C99 or GNU99 standard by using the flag `-std=gnu99` or `-std=c99`.

---

### Looping with `while`

The `while` statement in C is similar to the `while` statement in other languages:

```c
while (expr) {
    // Statement(s) to execute
}
```

The expression `expr` is evaluated. If it evaluates to `true`, the body of the `while` loop is executed; otherwise the loop ends. If the body has only one statement, braces aren't required.

---

### Looping with `do...while`

The `do` statement is similar to the `while` statement with one crucial difference: the expression is evaluated **after** the body has been executed.

```c
do {
    // Statement(s) to execute
} while (expr);
```

This means that a `do` loop will execute **at least once**, even if `expr` is false the first time.

---

### Breaking Out of a Loop Using `break`

The `break` statement provides a way to break out of a loop as soon as a desired condition is met, rather than waiting for the loop's condition to become false.

```c
#include <stdio.h>

int main(void) {
  int boundary;
  int denominator;
  int count = 0;
  int max = 5;

  printf("Enter a positive integer whose multiples you want printed: ");
  scanf("%d", &denominator);
  printf("Enter another positive integer up to which you want to see multiples printed: ");
  scanf("%d", &boundary);
  printf("Here are all the multiples of %d between 1 & %d\n", denominator, boundary);

  for(int i = 1; i <= boundary; i++){
    if(i % denominator == 0 && count < max){
      printf("%d ", i);
      count++;
      if(count == max){
        break;
      }
    }
  }
  printf("\n");
  return 0;
}
```

---

### Skipping to the Next Iteration Using `continue`

The `continue` statement causes a loop to skip ahead to the next iteration without breaking out of the loop. Any statements in the body of the loop that come after the `continue` statement will be skipped. This is used when you want to skip some statements in the body of the loop when a certain condition holds.

---

## Scope of a Variable: Local vs. Global

The **scope** of a variable in C determines where the variable is accessible.

- A **local variable** is accessible only within the block (`{ }`) in which it is defined.
- A **global variable** is defined outside any function and can be accessed everywhere in the program.

```c
#include <stdio.h>

// Global variable
int denominator;

/* Return 1 if divisible, 0 otherwise */
int isDivisible(int i){
  if (i % denominator == 0){
    return 1;
  } else {
    return 0;
  }
}

/* Function: main */
int main(void) {
  // Local variable
  int numerator;

  printf("Enter a positive integer: ");
  scanf("%d", &numerator);
  printf("Enter another positive integer: ");
  scanf("%d", &denominator);
  if(isDivisible(numerator) == 1){
    printf("%d is divisible by %d\n", numerator, denominator);
  } else {
    printf("%d is not divisible by %d\n", numerator, denominator);
  }
}
```

- `numerator` is a **local variable** — it can only be used inside `main`.
- `denominator` is a **global variable** — it is used in both `main` and `isDivisible`.
- A variable declared in a `for` loop (e.g., `int k`) is local to that loop and cannot be accessed anywhere else.

---

## Static Variables

**Static variables** in C are initialized just once and remain in memory until the end of the program. If a static variable is defined inside a function, it retains its value across multiple invocations of the function.

```c
#include <stdio.h>

void staticVarBehavior(){
  // Value retained across invocations
  static int staticVar = 1;

  // Re-initialized every time the function is called
  int nonStaticVar = 1;

  nonStaticVar++;
  printf("nonStaticVar = %d\n", nonStaticVar);

  staticVar++;
  printf("staticVar = %d\n", staticVar);
}

int main(void) {
  staticVarBehavior();
  staticVarBehavior();
  staticVarBehavior();
}
```

A static variable is declared as follows:

```c
static data_type variableName;
```

where `data_type` is the type of the variable, e.g., `int`, `char`, `float`, etc.

---

## Exercise

Write a program that prompts the user for an integer between 1 and 20 and then prints the factorial of that integer.

---

## Additional Resources

- [C for Python Programmers](http://www.cburch.com/books/cpy/) — Carl Burch. Hendrix College, 2011.
- [The C Book](https://publications.gbdirect.co.uk/c_book/) — Mike Banahan, Declan Brady, and Mark Doran, 1991.
