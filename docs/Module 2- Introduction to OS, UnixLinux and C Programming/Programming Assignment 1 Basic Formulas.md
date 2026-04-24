# Programming Assignment 1: Basic Formulas

> **Due:** Jan 19 by 11:59pm | **Points:** 100

## Introduction

In this assignment, you'll write a program that introduces you to the creation of a simple C program using the 5 basic constructs of a programming language: **assignment**, **input**, **output**, **conditional statements**, and **loops**.

> **Note:** Be sure to periodically review the thread *FAQ and Tips: PA1, Quiz 1 and Setup* in the Ed discussion board.

---

## Learning Outcomes

- How to write a C program to solve a problem (Module 1 MLO 4)
- How do you interact with the user in C programs? (Module 1 MLO 6)
- How are C programs transformed into an executable form? (Module 1 MLO 7)
- Define programming language constructs in C (Module 2 MLO 1)
- Describe structure data types of C programming language (Module 2 MLO 3)

---

## Instructions

Write a C program to calculate the **total surface area** and **volume** of several **spherical segments** given the sizes of their radius **R**, and the heights **h_a** and **h_b**.

A spherical segment is a portion of a sphere cut between two planes that are parallel to the equator of the sphere at distances h_a and h_b from the center. The following condition applies: **h_b ≤ h_a ≤ R**.

### Formulas

- **Top Surface Area** = π(R² − h_a²)
- **Bottom Surface Area** = π(R² − h_b²)
- **Lateral Surface Area** = 2πR(h_a − h_b)
- **Total Surface Area** = Top Surface Area + Bottom Surface Area + Lateral Surface Area
- **Volume** = (π/3)(h_a − h_b)(3R² − h_a² − h_a·h_b − h_b²)

---

## Program Requirements

1. Select appropriate identifiers and data types for all variables. Declare all of them correctly.
2. At the beginning of the program, request the user for a number between **2 and 10**, inclusively. Let us call this number **n**. The program must keep asking for this number until a value within that required range is provided.
3. The program will have a loop to obtain the parameters for the dimensions of **n** spherical segments from the user and calculate their dimensions.
4. There are three parameters for the dimensions:
   - **R** — the radius of the sphere
   - **h_a** — the height of the top area of the spherical segment
   - **h_b** — the height of the bottom area of the spherical segment
   
   All input values must be obtained using `scanf` in that exact order. After receiving all three values, they must be printed with appropriate labels. Then they must be validated:
   - R, h_a, and h_b must be **positive real values**
   - h_a and h_b must never be bigger than R
   - h_a should always be ≥ h_b
   
   If any condition is violated, print a message indicating the violation, reject the parameters, and request new values.
5. Evaluate the provided formulas to obtain the total surface area and volume for each spherical segment.
6. Print the output values with appropriate labels.
7. At the end of the loop, report the **average values** for all total surface areas and all volumes.
8. Include appropriate comments in the program.
9. The program should compile and run.

### Notes

- Use the value `3.14159265359` for π.
- For R², simply multiply the variable by itself: `R * R`.
- Use the `sqrt()` function for square roots. This requires:
  ```c
  #include <math.h>
  ```
  And compiling with the `-lm` flag:
  ```bash
  gcc -o t try.c -lm
  ```

---

## Sample Program Execution

```
$ gcc -o t try.c -lm
$ ./t
How many spherical segments you want to evaluate [2-10]? 
-3
How many spherical segments you want to evaluate [2-10]? 
15
How many spherical segments you want to evaluate [2-10]? 
2
Obtaining data for spherical segment number 1
What is the radius of the sphere (R)? 
10
What is the height of the top area of the spherical segment (ha)? 
7
What is the height of the bottom area of the spherical segment (hb)? 
4
Entered data: R = 10.00 ha = 7.00 hb = 4.00.
Total Surface Area = 612.61 Volume = 650.31.
Obtaining data for spherical segment number 2
What is the radius of the sphere (R)? 
10
What is the height of the top area of the spherical segment (ha)? 
4
What is the height of the bottom area of the spherical segment (hb)? 
7
Entered data: R = 10.00 ha = 4.00 hb = 7.00.
Invalid Input: ha = 4.00 hb = 7.00. ha must be greater than or equal to hb.
Obtaining data for spherical segment number 2
What is the radius of the sphere (R)? 
0
What is the height of the top area of the spherical segment (ha)? 
7
What is the height of the bottom area of the spherical segment (hb)? 
3
Entered data: R = 0.00 ha = 7.00 hb = 3.00.
Invalid Input: R = 0.00 ha = 7.00. R must be greater than or equal to ha.
Obtaining data for spherical segment number 2
What is the radius of the sphere (R)? 
10
What is the height of the top area of the spherical segment (ha)? 
7
What is the height of the bottom area of the spherical segment (hb)? 
3
Entered data: R = 10.00 ha = 7.00 hb = 3.00.
Total Surface Area = 697.43 Volume = 925.72.
Total average results:
Average Surface Area = 655.02 Average Volume = 788.02.
```

---

## Hints

- Code the functionality **incrementally**, constantly testing it.
- Write a program to solve one spherical segment first, then add `if` statements for validation, then add the loop.
- To print an `int`, use `"%d"` in the format string.
- To print a `float` or `double` to two decimal places, use `"%.2f"`.

---

## Submission

- You can only use **C** for coding this assignment and must use the **gcc** compiler.
- The assignment will be graded using **GradeScope**.
- Submit a single `.c` file named `LastNameFirstNameYourOnid_prog1.c`.

---

## Grading Rubric (100 pts)

| Criterion | Points |
|-----------|--------|
| **Echoing Input Data** — Program prints back R, h_a, h_b | 10 |
| **Number of Spherical Segments** — Loop requests number until valid | 10 |
| **Loop for Calculations** — Loop drives input and calculations | 10 |
| **Input Variables** — Dimensions retrieved correctly | 10 |
| **Validating Input Data** — Errors detected and reported | 20 |
| **Calculations** — Correct surface area and volume results | 10 |
| **Averages** — Correct average calculations and display | 10 |
| **Compilation** — Program compiles and produces results | 20 |
