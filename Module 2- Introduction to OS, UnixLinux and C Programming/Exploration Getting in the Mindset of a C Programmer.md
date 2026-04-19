# Exploration: Getting in the Mindset of a C Programmer

C is the standard language for operating systems and systems programming. Programming in C is very different from other languages like Python or C++. Whereas Python and C++ have thousands of individual library functions and extensive language documentation, a motivated student could read through the majority of the C standard in an hour or two.

---

## Learning the C Language

Take some time to read through the [C Language](https://en.cppreference.com/w/c/language) documentation to understand the syntax, operators, preprocessor directives, etc. The preprocessor macros can be a bit confusing, so don't worry if that part is over your head for now.

You may notice that some things have **(C99)** or **(C11)** after them. This denotes which standard they first appeared in as the language evolved. We are using the **C99** standard in this course, so some of the features that C11, C17, and C23 added are irrelevant — you can skip these parts. This decision is not arbitrary: **C99 is the standard used for the vast majority of operating system source code and systems programming**, and the additions of subsequent standards are fairly minor.

---

## Essential Standard Library Knowledge

Next, look over the methods available for:

- [File input/output](https://en.cppreference.com/w/c/io)
- [Null-terminated byte strings](https://en.cppreference.com/w/c/string/byte)

Reading and writing to files and string manipulation are two fundamental skills for C programming, and a significant amount of your assignments' code will be devoted to these tasks. You don't need to memorize all of this, but you should have a general idea of what kinds of functionality the standard library provides and where you could find the information to actually use these functions.

> **You should constantly refer to documentation** to ensure you are properly calling functions, checking return values, and handling errors.

---

## Safe C Programming Practices

Another major difference between C and other languages is that safe C programming requires:

- **Explicit error/return value checking**
- **Careful memory management**
- Programs should be broken down into **very small functions**, each with clearly defined arguments, return values, and effects on program state (such as modifying global variables)
- A general rule of thumb: an individual function should be **short enough to fit in a terminal window**

Throughout this course, the content will highlight useful functions and commands. You should regularly refer to the **manual pages** for each one to understand how to use them properly.
