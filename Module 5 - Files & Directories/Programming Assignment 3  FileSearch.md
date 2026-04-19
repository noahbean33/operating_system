# Programming Assignment 3: FileSearch

> **Due:** Feb 9 by 11:59pm | **Points:** 110

## Introduction

In this assignment, you'll write a program to get familiar with **reading and writing files and directories** on Unix.

## Learning Outcomes

- Describe the API for operations related to files (Module 5 MLO 2)
- Describe the API for operations related to directories (Module 5 MLO 5)
- What are scopes and types of permissions associated with files and directories? (Module 5 MLO 6)

---

## Instructions

Write a program that:

1. Reads directory entries
2. Finds a file in the current directory based on user-specified criteria
3. Reads and processes the data in the chosen file
4. Creates a directory
5. Creates new files in the new directory and writes processed data to them

---

## CSV File Format

Same format as Assignment 2: **Title**, **Year**, **Languages**, **Rating Value**.

---

## Program Functionality

### Main Menu

```
1. Select file to process
2. Exit the program
Enter a choice 1 or 2:
```

### Selecting a File to Process

```
Which file you want to process?
Enter 1 to pick the largest file
Enter 2 to pick the smallest file
Enter 3 to specify the name of a file
Enter a choice from 1 to 3:
```

- **Option 1:** Find the **largest** `.csv` file starting with `movies_`
- **Option 2:** Find the **smallest** `.csv` file starting with `movies_`
- **Option 3:** User enters a filename; if not found, show error and re-prompt (stay at file menu)

After successful selection: `Now processing the chosen file named <filename>`

### Processing the File

1. **Create a directory** named `your_onid.movies.random` (random: 0–99999)
   - Permissions: `rwxr-x---` (0750)
   - Print: `Created directory with name <dirname>`

2. **Create one file per year** with at least one movie released
   - Filename: `YYYY.txt`
   - Permissions: `rw-r-----` (0640)
   - Contents: one movie title per line

After processing, return to the main menu.

---

## Sample Execution

```
1. Select file to process
2. Exit the program

Enter a choice 1 or 2: 1

Which file you want to process?
Enter 1 to pick the largest file
Enter 2 to pick the smallest file
Enter 3 to specify the name of a file

Enter a choice from 1 to 3: 1
Now processing the chosen file named movies_1.csv
Created directory with name chaudhrn.movies.89383

1. Select file to process
2. Exit the program

Enter a choice 1 or 2: 1

Enter a choice from 1 to 3: 3
Enter the complete file name: foo_bar
The file foo_bar was not found. Try again

Enter a choice from 1 to 3: 3
Enter the complete file name: great_movies.csv
Now processing the chosen file named great_movies.csv
Created directory with name chaudhrn.movies.92777

Enter a choice 1 or 2: 2
```

---

## Hints & Resources

- Code incrementally and use version control
- [`random()` / `srandom()`](https://man7.org/linux/man-pages/man3/random.3.html) — generate random numbers
- [`sprintf()`](https://man7.org/linux/man-pages/man3/sprintf.3.html) — write formatted output to a string

---

## Submission

- Graded using **GradeScope**
- Submit zip: `youronid_assignment3.zip` (or single `.c` file: `youronid_assignment3.c`)
- Compilation: `gcc --std=gnu99 -o file_search *.c`

---

## Grading Rubric (110 pts)

| Criterion | Points |
|-----------|--------|
| Correct options displayed for picking a file | 5 |
| Largest file option works correctly | 15 |
| Smallest file option works correctly | 15 |
| Pick file by name works correctly | 15 |
| Correct directory created each time | 15 |
| Correct number of files created | 10 |
| File names are correct | 5 |
| Directory and file permissions are correct | 10 |
| Each file has all movies for that year | 15 |
| Program exits only on correct choice | 4 |
| Invalid top-level choice handled | 1 |
