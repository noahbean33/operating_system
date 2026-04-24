# Exploration: Unix Shell

## What is the Shell?

The **shell** is a user-interface to the OS. It is a text-based, command-line interpreter. The shell provides access to all Unix user-level programs. Using the shell you can:

- Start programs
- Kill programs
- Connect processes together (using pipes)
- Manage I/O to and from processes
- Create, delete, and manage files and directories

There are many different shells that are typically installed on Unix/Linux distributions. These shells (and the standard path to the shell command on most installations) include:

| Shell | Path |
|-------|------|
| **Bourne Shell** | `/bin/sh` |
| **C-Shell** | `/bin/csh` |
| **BASH** ("Bourne-again shell") | `/usr/local/bin/bash` or `/bin/bash` |
| **TCSH** (enhanced C-Shell) | `/usr/local/bin/tcsh` or `/bin/tcsh` |
| **Korn Shell** | `/bin/ksh` |

In this class, we will use the **BASH shell**, commonly written as `bash`.

---

## Common Unix Commands

Let's now look at some common Unix commands related to files and directories.

### Accessing Online Reference Manual — `man`

Unix systems provide access to an online reference manual via the `man` command. This command can be used to read the reference manual for shell commands, system calls, and many functions in the standard C library. For example, to view the reference for the `cp` command:

```bash
$ man cp
```

### Directory/File Management

| Command | Description |
|---------|-------------|
| `pwd` | **Print working directory** — Which directory am I in? |
| `cd` | **Change directory** — Moves your current working directory to a different one |
| `ls` | Display the files in a given directory |
| `mkdir` | Create a directory |
| `rmdir` | Remove a directory |
| `rm` | Remove files (and directories if used recursively) |
| `mv` | Move or rename files and directories |
| `cp` | Copy files and directories |
| `chmod` | **Change mode** — Change the permissions of files or directories |

### File Viewing and Selecting

| Command | Description |
|---------|-------------|
| `cat` | Concatenate character data stored in a file with another file; primary use is to dump data to the terminal |
| `more` | Take character data and display one screen-full at a time |
| `less` | Similar to `more` |
| `head` | Display the beginning of a text file |
| `tail` | Display the (tail) end of a text file |
| `grep` | Search a text file |

---

## Shell Scripting

All the commands that are accessible from the shell can be placed in a **shell script**. Shell scripts are executed line by line, as if the lines were being typed in one by one. Shell scripts provide the features of high-level programming languages, such as variables, conditional expressions, loops, etc.

Shell scripts are commonly used to:

- **Automate frequent tasks** and simplify complex commands
- Run nightly backups of data or start up servers
- Create small programs quickly or programs that need to change frequently
- Act as **glue** to connect together other programs

---

## Exercise

The `wc` command prints various counts about data in a text file. Read the [man page for `wc`](https://man7.org/linux/man-pages/man1/wc.1.html) for details. What options can you use with `wc` to print the longest line in the text file `wc_test.txt`?

---

## Additional Resources

- [List of Unix commands](https://en.wikipedia.org/wiki/List_of_Unix_commands)
- [Bash scripting cheat sheet](https://devhints.io/bash)
- [Linux man pages online](https://man7.org/linux/man-pages/)
