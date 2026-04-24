# Exploration: Debugging C

## Introduction

Modern IDEs (Integrated Development Environments) such as Visual Studio, Eclipse, and Visual Studio Code come with excellent tools for code generation, compilation, debugging, and stepping through live code. However, most Unix systems come with the debugger **gdb** installed. Additionally, the tool **valgrind** is helpful in finding memory leaks in C programs on Unix.

---

## gdb

**gdb** is a debugger we can use with gcc. We need to compile our program with the `-g` flag, which causes gcc to add debugging information in the executable. We can then start the debugger on the executable:

```bash
gcc --std=gnu99 -g testit.c -o testit
gdb ./testit
```

### Key gdb Commands

| Command | Description |
|---------|-------------|
| `run` | (Re)starts the program; stops at breakpoints. Can add args, e.g., `run 6 myfile` |
| `break lineNum` | Set a breakpoint at line `lineNum` |
| `info breakpoints` | Show the current breakpoints |
| `clear lineNum` | Clear the breakpoint at line `lineNum` |
| `step` | Execute a single line of C code; **will enter** a function call |
| `next` | Execute a single line of C code; **will not enter** a function call |
| `continue` | Continue execution until the next breakpoint or program completion |
| `jump lineNum` | Jump to just before `lineNum` |
| `list` | Print some lines before and after the current stop point |
| `print expr` | Print the value of an expression (e.g., a variable) |
| `watch expr` | Watch an expression; gdb stops when its value changes |
| `quit` | Stop debugging (exit gdb) |

---

## valgrind

**valgrind** is a tool that helps find memory leaks in C programs. Compile the program with the `-g` flag, then run with valgrind:

```bash
gcc --std=gnu99 -g -o leaky leaky.c
valgrind --leak-check=yes --show-reachable=yes ./leaky
```

### Useful valgrind Flags

| Flag | Description |
|------|-------------|
| `--leak-check=yes` | Turn on detailed memory leak checker |
| `--show-reachable=yes` | Check if unfreed memory is reachable |
| `--track-origins=yes` | Keep track of origins of uninitialized values |
| `--log-file` | Write output to a log file |

---

## Additional Resources

- gdb cheat sheets: [1](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf), [2](https://cs.brown.edu/courses/cs033/docs/guides/gdb.pdf), [3](https://gabriellesc.github.io/teaching/resources/GDB-cheat-sheet.pdf)
- [GCC Debugging Options](https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html)
- [GDB Project Home Page](https://www.sourceware.org/gdb/)
- [Valgrind Home Page](https://valgrind.org/)
