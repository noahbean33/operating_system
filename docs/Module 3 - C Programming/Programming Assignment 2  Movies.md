# Programming Assignment 2: Movies

> **Due:** Feb 2 by 11:59pm | **Points:** 100

## Introduction

In this assignment, you'll write a program that will help you learn C concepts such as **pointers**, **structures**, and **linked lists**.

---

## Learning Outcomes

- How to define and use structures in C (Module 3 MLO 2)
- What are pointers and how is this mechanism supported in C? (Module 3 MLO 3)
- Describe memory allocation in Unix (Module 3 MLO 4)
- Explain how to create and manipulate strings in C (Module 4 MLO 1)

> **Note:** This assignment draws on material from Module 3 and Module 4, in addition to earlier modules. We highly recommend taking all the non-graded quizzes in Module 3 and Module 4 before you start coding.

---

## Instructions

Write a program that:

1. Reads a CSV file with movie data (provided as a command-line argument)
2. Processes the data to create structs for each movie
3. Creates a **linked list** of all these structs
4. Gives user choices to ask questions about the movies
5. Prints out the data per user choice

---

## Format of the CSV File

The first line is always the **header line** (column headers, not movie data). All other lines have valid movie data with no missing columns or empty lines.

**Assumptions:**
- No maximum number of movies
- Commas appear only as delimiters, never in values
- File name < 50 characters, no spaces

### Columns

| Column | Description |
|--------|-------------|
| **Title** | String with the movie title (no max length assumed). E.g., `Iron Man 2` |
| **Year** | 4-digit integer (1900–2021 inclusive). E.g., `2010` |
| **Languages** | One or more strings enclosed in `[]`, separated by `;`. Max 5 languages, max 20 chars each. E.g., `[English;Portuguese;Spanish]` |
| **Rating Value** | Number between 1 and 10 (integer or one decimal). E.g., `8.7` |

---

## Program Functionality

### Process the Input File

After processing the file, print:

```
Processed file XYZ and parsed data for M movies
```

### Interactive Menu

1. **Show movies released in the specified year** — Ask for a year, display matching movie names (one per line). If none: `No data about movies released in the year YYYY`
2. **Show highest rated movie for each year** — For each year with data, show one movie with the highest rating: `YYYY RatingValue MovieTitle`
3. **Show movies for a specific language** — Ask for a language (exact match), display year and title. If none: `No data about movies released in <language>`
4. **Exit**

> For invalid menu choices, print: `You entered an incorrect choice. Try again.`

---

## Technical Requirements

- Read data from the file **line by line**
- Break up each line into **tokens**
- Create a `struct movie` with the data for each line
- Create a **linked list** containing all the movie structs

---

## Sample Program Execution

```
$ ./movies movies_sample_1.csv
Processed file movies_sample_1.csv and parsed data for 24 movies

1. Show movies released in the specified year
2. Show highest rated movie for each year
3. Show the title and year of release of all movies in a specific language
4. Exit from the program

Enter a choice from 1 to 4: 1
Enter the year for which you want to see movies: 2012
The Avengers
Rise of the Guardians
Anna Karenina

Enter a choice from 1 to 4: 2
2008 7.9 Iron Man
2009 7.6 Sherlock Holmes
2010 7.0 Iron Man 2
...

Enter a choice from 1 to 4: 3
Enter the language for which you want to see movies: English
2008 The Incredible Hulk
2009 Sherlock Holmes
...

Enter a choice from 1 to 4: 4
$
```

---

## Hints

- Code incrementally and use **version control** (e.g., Git).
- [`atoi()`](https://man7.org/linux/man-pages/man3/atoi.3.html) — Convert string to int.
- [`strtof()`](https://man7.org/linux/man-pages/man3/strtof.3.html) — Convert string to float.
- [`strtok_r()`](https://man7.org/linux/man-pages/man3/strtok_r.3.html) — Extract tokens from a string (less error-prone than `strtok()`).
- `"%.1f"` — Print a double to one decimal place.

---

## Submission

- Graded using **GradeScope**
- Submit a **zip file** named `youronid_assignment2.zip` (or a single `.c` file named `youronid_assignment2.c`)
- All code files must be at the **root level** of the zip
- Compilation command: `gcc --std=gnu99 -o movies *.c`

---

## Grading Rubric (100 pts)

| Criterion | Points |
|-----------|--------|
| Processed file message is correct | 15 |
| "Show movies released in the specified year" works correctly | 15 |
| "Show highest rated movie for each year" works correctly | 20 |
| "Show movies in a specific language" works correctly | 20 |
| Program exits only on correct choice | 4 |
| Incorrect top-level choice handled | 1 |
| File processed line by line | 8 |
| Appropriate movie struct defined and used | 8 |
| Linked list used for all movie structs | 9 |
