# Exploration: Hello World

Now that we have some very basic proficiency with the command line interface, let's go ahead and write our first program on os1! To do this, we will use an extremely powerful text editor called **vim**. Vim is the standard text editor on \*nix, widely available, and highly extensible. An experienced vim user can out-code anyone on a graphical IDE, and vim has a very powerful scripting language and regular expression support for automating large changes to files.

---

## Learning to Use Vim

> *"I've been using Vim for about 2 years now, mostly because I can't figure out how to exit it."*

The downside of vim is that it is known for having a very steep learning curve. Even exiting vim can be confusing for a first-time user, a source of endless memery on the internet. The good news is, once you learn it, you'll never want to go back to anything else. It's one of the most popular IDEs in the world, and basically dominates all serious systems-level programming as a de facto standard.

Vim is a **modal text editor**, which means you switch between different modes to do different things (selecting text, using keyboard shortcuts, entering text, running commands, etc.). Most of you are used to using modeless text editors, so don't be surprised if vim takes a little bit of getting used to. Generally, if you get yourself into trouble, hit `Escape` until you get back to **Normal mode**.

---

## The Vim Tutorial

Before we get started, let's go ahead and run through the vim tutorial. If you log into os1 and enter the command `vimtutor`, you'll be taken to a tutorial that will guide you to a basic level of proficiency with vim. The tutorial is basically just a text file that opens in vim, and you follow the instructions in it to learn to move around, modify text, etc.

- This tutorial takes around **30 minutes** to complete.
- By the end of it you should feel comfortable editing a simple file — which is really all you need to make it through this class.
- As you go through the course, feel free to ask questions about how to do things in vim, and definitely make use of Google as well.
- Vim also has a built-in help system that you can access by pressing `<F1>` or with the `:help` command.

Later in the course, we will look at configuring vim to make it easier to use, but let's just go ahead and write our first file to start.

---

## Writing Your First Program

1. Log into os1 and go to your home directory if you aren't already there. Remember, you can just use the `cd` command with no arguments to go home.

2. Create a directory for your project:
   ```bash
   mkdir ~/hello-world/
   ```

3. Change into that directory:
   ```bash
   cd ~/hello-world
   ```

4. Create and open a new file in vim:
   ```bash
   vim hello-world.c
   ```

5. In vim, go into **Insert mode** (press `i`), and enter the following program. To paste into your terminal:
   - **Linux**: `Ctrl+Shift+V`
   - **WSL**: `Ctrl+Shift+V` / `Ctrl+Shift+C`
   - **Mac**: `⌘+V`

```c
#include <stdlib.h>
#include <stdio.h>

int
main()
{
    printf("Hello World!\n");
    return EXIT_SUCCESS;
}
```

6. Save the file and exit: press `Escape` to go to Normal mode, then `:` to enter Command mode, and type `wq` ("write and quit").

Your directory structure should now look like:

```
~/hello-world/
└── hello-world.c
```

---

## Compiling Your First Program

Compile using the **gcc** compiler. In this class, we always compile against the **C99** standard (specified with `-std`), and we name our output executable with `-o`:

```bash
gcc -std=c99 hello-world.c -o hello-world
```

- You shouldn't see any output when you do this — you will simply see the next prompt.
- If your program has a typo or other error, you may see warnings or errors. Go back and check that you entered it correctly in vim!

Your directory structure should now look like:

```
~/hello-world/
├── hello-world.c
└── hello-world
```

---

## Running Your First Program

Simply run your `hello-world` executable:

```bash
./hello-world
```

> **Note the leading `./`**: By default, the shell searches some system directories (like `/bin` and `/sbin`) for executables when entered by name without a path. It does **not** search the current directory for safety and security reasons. You have to intentionally tell the shell "run `hello-world` in the current directory."

If you were to just enter `hello-world`, you would get a "command not found" error. This can be really annoying when you are testing something out — for example, if you name your program `test.c` and compile it to an executable named `test`. When you go to run `test`, you see no output and no error, because the shell is running `/bin/test`, not the one you just compiled. You'd need to run `./test` to run your own program!

> **Note:** By default, IT supplies students with a `~/.bashrc` file that adds the current directory to the `PATH` environment variable, so if you leave your `~/.bashrc` the way you found it, you'll be able to just type `hello-world` to run your program. *This is a serious security issue and IT should know better.*

Expected output:

```
$ ./hello-world
Hello World!
```
