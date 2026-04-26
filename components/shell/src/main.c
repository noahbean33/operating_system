
#include <stdio.h>         // For printing and input
#include <stdlib.h>        // For exit and memory
#include <string.h>        // For string operations
#include <unistd.h>        // For fork, execvp, chdir
#include <sys/types.h>     // For pid_t type
#include <sys/wait.h>      // For waitpid to wait on processes
#include <fcntl.h>         // For open and dup2
#include <signal.h>        // For handling signals like SIGINT
#include <errno.h>         // For error numbers
#include <stdbool.h>      // For bool (C23 keyword, header kept for portability)

// Macros 
// Your shell must support command lines with a maximum length of 2048 characters, and a maximum of 512 arguments.
#define MAX_CMD_LENGTH 2048              // Max length of a command line
#define MAX_ARGS 512                     // Max number of arguments
#define MAX_BG_PROCS 100                 // Max background processes
#define ENTERING_MSG "\nEntering foreground-only mode (& is now ignored)\n: "  // Message for entering foreground-only mode
constexpr int ENTERING_LEN = sizeof(ENTERING_MSG) - 1;          // Length of entering message
#define EXITING_MSG "\nExiting foreground-only mode\n: "  // Message for exiting foreground-only mode
constexpr int EXITING_LEN = sizeof(EXITING_MSG) - 1;           // Length of exiting message

// Global Variables - things shared across the program
int last_status = 0;             // Stores exit status of last foreground process
volatile sig_atomic_t foreground_only_mode = 0;    // Flag for foreground-only mode (1 = on)
pid_t background_processes[MAX_BG_PROCS];  // Array of background process IDs
int background_count = 0;        // Number of background processes
char command_line[MAX_CMD_LENGTH];  // Buffer for user input
char *arguments[MAX_ARGS + 1];   // Array of command arguments
char *input_file = nullptr;         // File for input redirection
char *output_file = nullptr;        // File for output redirection
bool run_in_background = false;       // Flag for background execution (1 = yes)
int argument_count = 0;          // Number of arguments parsed
pid_t child_pid;                 // PID of the child process

// Forward declarations
// Forward declarations for all functions in smallsh.c

void handle_signal_tstp(int signal_number);
void set_ignore_signal_int(void);
void set_signal_tstp(void);
void setup_signals(void);
void check_one_background(int i);
void check_background(void);
void reset_globals(void);
void check_token(char *token);
void parse_next_token(char *token);
void parse_input(void);
void set_child_signal_int(int bg);
void set_child_signal_tstp(void);
void setup_child_signals(void);
void redirect_input(void);
void redirect_output(void);
[[noreturn]] void execute_child(void);
void handle_background(void);
void handle_foreground(void);
void execute_command(void);
void show_prompt(void);
void get_input(void);
bool is_valid_input(void);
void kill_background(void);
[[noreturn]] void execute_exit(void);
void execute_cd(void);
void execute_status(void);
void run_command(void);
void main_loop(void);
int main(void);

/*
* Function: handle_signal_tstp
* Description: Handles SIGTSTP signal to toggle foreground-only mode
* Parameters:
*     - signal_number: The signal number
* Return: None
*/
void handle_signal_tstp(int signal_number)
{
    // Switch mode and print message
    if (foreground_only_mode == 0)
    {
        foreground_only_mode = 1;
        write(STDOUT_FILENO, ENTERING_MSG, ENTERING_LEN);  // Print entering foreground-mode message
    }
    else
    {
        foreground_only_mode = 0;
        write(STDOUT_FILENO, EXITING_MSG, EXITING_LEN);  // Print exiting foreground-mode message
    }
}

/*
7. Signals SIGINT & SIGTSTP
SIGINT
A CTRL-C command from the keyboard sends a SIGINT signal to the parent process and all children at the same time (this is a built-in part of Linux).

Your shell, i.e., the parent process, must ignore SIGINT.
Any children running as background processes must ignore SIGINT.
A child running as a foreground process must terminate itself when it receives SIGINT.
The parent must not attempt to terminate the foreground child process; instead the foreground child (if any) must terminate itself on receipt of this signal.
If a child foreground process is killed by a signal, the parent must immediately print out the number of the signal that killed it's foreground child process (see the example) before prompting the user for the next command.
SIGTSTP
A CTRL-Z command from the keyboard sends a SIGTSTP signal to your parent shell process and all children at the same time (this is a built-in part of Linux).

A child, if any, running as a foreground process must ignore SIGTSTP.
Any children running as background process must ignore SIGTSTP.
When the parent process running the shell receives SIGTSTP
The shell must display an informative message (see below) immediately if it's sitting at the prompt, or immediately after any currently running foreground process has terminated
The shell then enters a state where subsequent commands can no longer be run in the background.
In this state, the & operator must simply be ignored, i.e., all such commands are run as if they were foreground processes.
If the user sends SIGTSTP again, then your shell will
Display another informative message (see below) immediately after any currently running foreground process terminates
The shell then returns back to the normal condition where the & operator is once again honored for subsequent commands, allowing them to be executed in the background.
See the example below for usage and the exact syntax which you must use for these two informative messages.
*/

/*
* Function: set_ignore_signal_int
* Description: Sets SIGINT to be ignored by the shell
* Parameters: None
* Return: None
*/
void set_ignore_signal_int()
{
    struct sigaction action = {0};
    action.sa_handler = SIG_IGN;  // Ignore SIGINT
    sigfillset(&action.sa_mask);  // Block other signals during handler
    sigaction(SIGINT, &action, nullptr);  // Apply the setting
}

/*
* Function: set_signal_tstp
* Description: Sets up the SIGTSTP handler for the shell
* Parameters: None
* Return: None
*/
void set_signal_tstp()
{
    struct sigaction action = {0};
    action.sa_handler = handle_signal_tstp;  // Use handler
    sigfillset(&action.sa_mask);  // Block other signals
    action.sa_flags = SA_RESTART;  // Restart interrupted syscalls (e.g. fgets)
    sigaction(SIGTSTP, &action, nullptr);  // Set it up
}

/*
* Function: setup_signals
* Description: Sets up signal handlers for SIGINT and SIGTSTP
* Parameters: None
* Return: None
*/
void setup_signals()
{
    set_ignore_signal_int();  // Ignore SIGINT
    set_signal_tstp();        // Handle SIGTSTP
}

/*
* Function: check_one_background
* Description: Checks if a background process finished
* Parameters:
*     - i: Index in background_processes array
* Return: None
*/
void check_one_background(int i)
{
    int status;
    pid_t pid = waitpid(background_processes[i], &status, WNOHANG);  // Check without waiting

    if (pid > 0)  // Process is done
    {
        if (WIFEXITED(status))
        {
            printf("background pid %d is done: exit value %d\n", pid, WEXITSTATUS(status));  // Normal exit
        }
        else
        {
            if (WIFSIGNALED(status))
            {
                printf("background pid %d is done: terminated by signal %d\n", pid, WTERMSIG(status));  // Killed by signal
            }
        }
        fflush(stdout);
        /*
        Be sure you flush out the output buffers each time you print, 
        as the text that you're outputting may not reach the screen until you do in this kind of interactive program. 
        To do this, call fflush() immediately after each and every time you output text.
        */
        // Shift array to remove finished process
        for (int j = i; j < background_count - 1; j++)
        {
            background_processes[j] = background_processes[j + 1];
        }
        background_count--;  // remove process
    }
}

/*
* Function: check_background
* Description: Checks all background processes for completion
* Parameters: None
* Return: None
*/
void check_background()
{
    for (int i = background_count - 1; i >= 0; i--)  // Iterate backwards to avoid skipping after removal
    {
        check_one_background(i); 
    }
}

/*
* Function: reset_globals
* Description: Resets global variables
* Parameters: None
* Return: None
*/
void reset_globals()
{
    argument_count = 0;    // Reset args
    input_file = nullptr;  // Reset input file
    output_file = nullptr; // Reset output file
    run_in_background = false; // Reset background
}

/*
* Function: check_token
* Description: Processes a token from the command line
* Parameters:
*     - token: A piece of the command
* Return: None
*/
void check_token(char *token)
{
    if (strcmp(token, "<") == 0)
    {
        input_file = strtok(nullptr, " ");  // Set input redirection
    }
    else if (strcmp(token, ">") == 0)
    {
        output_file = strtok(nullptr, " ");  // Set output redirection
    }
    else if (strcmp(token, "&") == 0)
    {
        if (!strtok(nullptr, " "))  // If & is last
        {
            run_in_background = true;  // Background mode
        }
    }
    else
    {
        arguments[argument_count++] = token;  // Add as argument
    }
}

/*
* Function: parse_next_token
* Description: Parses the next token
* Parameters:
*     - token: A string from the command
* Return: None
*/
void parse_next_token(char *token)
{
    if (token)
    {
        if (argument_count < MAX_ARGS)
        {
            check_token(token);  // Process it
        }
    }
}

/*
* Function: parse_input
* Description: Splits command line into arguments and flags
* Parameters: None
* Return: None
*/
void parse_input()
{
    reset_globals();  // Reset old global variables
    char *token = strtok(command_line, " ");  // First token
    while (token)
    {
        parse_next_token(token);  // Handle the token
        token = strtok(nullptr, " "); // Get next token
    }
    arguments[argument_count] = nullptr;  // End the array
}

/*
* Function: set_child_signal_int
* Description: Sets SIGINT behavior for child processes
* Parameters:
*     - bg: 1 if background, 0 if foreground
* Return: None
*/
void set_child_signal_int(int bg)
{
    struct sigaction sa = {0};
    sa.sa_handler = bg ? SIG_IGN : SIG_DFL;  // default to foreground and ignore background
    sigaction(SIGINT, &sa, nullptr);  // Set child SIGINT
}

/*
* Function: set_child_signal_tstp
* Description: Sets SIGTSTP to be ignored by children
* Parameters: None
* Return: None
*/
void set_child_signal_tstp()
{
    struct sigaction sa = {0};
    sa.sa_handler = SIG_IGN;  // Ignore SIGTSTP
    sigaction(SIGTSTP, &sa, nullptr);  // Set child TSTP
}

/*
* Function: setup_child_signals
* Description: Configures signals for child process
* Parameters: None
* Return: None
*/
void setup_child_signals()
{
    set_child_signal_int(run_in_background);  // SIGINT setup
    set_child_signal_tstp();                  // SIGTSTP setup
}

/*
* Function: redirect_input
* Description: Redirects stdin to a file or /dev/null
* Parameters: None
* Return: None
*/
void redirect_input()
{
    /*
    5. Input & Output Redirection
You must do any input and/or output redirection using dup2(). 
The redirection must be done before using exec() to run the command.

An input file redirected via stdin must be opened for reading only; 
if your shell cannot open the file for reading, it must print an error message and set the exit status to 1 
(but don't exit the shell).
Similarly, an output file redirected via stdout must be opened for writing only; 
it must be truncated if it already exists or created if it does not exist. 
If your shell cannot open the output file, it must print an error message and set the exit status to 1 
(but don't exit the shell).
Both stdin and stdout for a command can be redirected at the same time (see example below).
    */
    if (input_file)
    {
        int fd = open(input_file, O_RDONLY);  // Open input file

        if (fd == -1)
        {   // If file cannot be opened
            fprintf(stderr, "cannot open %s for input\n", input_file);
            exit(1);
        }
        dup2(fd, STDIN_FILENO);  // Redirect stdin
        close(fd);    // Close file
    }
    else
    {
        if (run_in_background)
        {
            int fd = open("/dev/null", O_RDONLY);  // Use /dev/null
            dup2(fd, STDIN_FILENO);  // Redirect stdin
            close(fd);    // Close file
        }
    }
}

/*
* Function: redirect_output
* Description: Redirects stdout to a file or /dev/null
* Parameters: None
* Return: None
*/
void redirect_output()
{
        /*
    5. Input & Output Redirection
You must do any input and/or output redirection using dup2(). 
The redirection must be done before using exec() to run the command.

An input file redirected via stdin must be opened for reading only; 
if your shell cannot open the file for reading, it must print an error message and set the exit status to 1 
(but don't exit the shell).
Similarly, an output file redirected via stdout must be opened for writing only; 
it must be truncated if it already exists or created if it does not exist. 
If your shell cannot open the output file, it must print an error message and set the exit status to 1 
(but don't exit the shell).
Both stdin and stdout for a command can be redirected at the same time (see example below).
    */
    if (output_file)
    {
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Open output file

        if (fd == -1)  // If file cannot be opened
        {
            fprintf(stderr, "cannot open %s for output\n", output_file);
            exit(1);  
        }
        dup2(fd, STDOUT_FILENO);  // Redirect stdout
        close(fd);    // Close file
    }
    else
    {
        if (run_in_background)
        {
            int fd = open("/dev/null", O_WRONLY);  // Use /dev/null
            dup2(fd, STDOUT_FILENO);  // Redirect stdout
            close(fd);    // Close it
        }
    }
}

/*
* Function: execute_child
* Description: Runs the command in the child process
* Parameters: None
* Return: None (exits)
*/
void execute_child()
{
    setup_child_signals();  // Set signals
    redirect_input();       // Handle input
    redirect_output();      // Handle output

    if (execvp(arguments[0], arguments) == -1)  // Run command
    {
        fprintf(stderr, "%s: no such file or directory\n", arguments[0]);
        exit(1); 
    }
}

/*
* Function: handle_background
* Description: Manages a background process
* Parameters: None
* Return: None
*/
void handle_background()
{
    printf("background pid is %d\n", child_pid);  // Print PID
    fflush(stdout);  
        /*
        Be sure you flush out the output buffers each time you print, 
        as the text that you're outputting may not reach the screen until you do in this kind of interactive program. 
        To do this, call fflush() immediately after each and every time you output text.
        */

    if (background_count < MAX_BG_PROCS)
    {
        background_processes[background_count++] = child_pid;  // Store PID
    }
}

/*
* Function: handle_foreground
* Description: Manages a foreground process
* Parameters: None
* Return: None
*/
void handle_foreground()
{
    waitpid(child_pid, &last_status, 0);  // Wait for child
    if (WIFSIGNALED(last_status))
    {
        printf("terminated by signal %d\n", WTERMSIG(last_status));  // Show signal if killed
    }
    fflush(stdout);  
        /*
        Be sure you flush out the output buffers each time you print, 
        as the text that you're outputting may not reach the screen until you do in this kind of interactive program. 
        To do this, call fflush() immediately after each and every time you output text.
        */
}

/*
* Function: execute_command
* Description: Forks and executes a command
* Parameters: None
* Return: None
*/
void execute_command()
{
    /*
    4. Executing Other Commands
Your shell will execute any commands other than the 3 built-in command by using fork(), exec() and waitpid()

Whenever a non-built in command is received, the parent (i.e., smallsh) will fork off a child.
The child will use a function from the exec() family of functions to run the command.
Your shell must use the PATH variable to look for non-built in commands, and it must allow shell scripts to be executed.
If a command fails because the shell could not find the command to run, then the shell will print an error message
 and set the exit status to 1
A child process must terminate after running a command (whether the command is successful or it fails).
    */
    child_pid = fork();  // Create child
    if (child_pid == 0)  // Child process
    {
        execute_child();  // Run it
    }
    else if (child_pid > 0)  // Parent process
    {
        if (run_in_background)
        {
            handle_background();  // Handle background
        }
        else
        {
            handle_foreground();  // Handle Foreground
        }
    }
}

/*
. The Command Prompt
Use the colon : symbol as a prompt for each command line. 

The general syntax of a command line is:

command [arg1 arg2 ...] [< input_file] [> output_file] [&]
where items in square brackets are optional.

You can assume that a command is made up of words separated by spaces.
The special symbols <, > and & are recognized, but they must be surrounded by spaces like other words.
If the command is to be executed in the background, the last word must be &. If the & character appears anywhere else, just treat it as normal text.
If standard input or output is to be redirected, the > or < words followed by a filename word must appear after all the arguments. Input redirection can appear before or after output redirection.
Your shell does not need to support any quoting; so arguments with spaces inside them are not possible. We are also not implementing the pipe "|" operator.
Your shell must support command lines with a maximum length of 2048 characters, and a maximum of 512 arguments.
You do not need to do any error checking on the syntax of the command line.
*/

/*
* Function: show_prompt
* Description: Displays the shell prompt
* Parameters: None
* Return: None
*/
void show_prompt()
{
    printf(": ");  // Show prompt colon
    fflush(stdout);  
        /*
        Be sure you flush out the output buffers each time you print, 
        as the text that you're outputting may not reach the screen until you do in this kind of interactive program. 
        To do this, call fflush() immediately after each and every time you output text.
        */
}

/*
* Function: get_input
* Description: Reads user input
* Parameters: None
* Return: None
*/
void get_input()
{
    fgets(command_line, MAX_CMD_LENGTH, stdin);  // Get the line
    command_line[strcspn(command_line, "\n")] = '\0';  // Remove newline
}

/*
* Function: is_valid_input
* Description: Checks if input is valid
* Parameters: None
* Return: 1 if valid, 0 if not
*/
bool is_valid_input()
{ // 2. Comments & Blank Lines
    // Any line that begins with the # character is a comment line and must be ignored. 
    // Mid-line comments, such as the C-style //, will not be supported.
    // A blank line (one without any commands) must also do nothing.
    // Your shell must just re-prompt for another command when it receives either a blank line or a comment line.
    return command_line[0] != '#' && command_line[0] != ' ' && command_line[0] != '\0';
}

/*
* Function: kill_background
* Description: Kills all background processes
* Parameters: None
* Return: None
*/
void kill_background()
{
    for (int i = 0; i < background_count; i++)  // Loop through all background processes
    {
        kill(background_processes[i], SIGTERM);  // Terminate each background process
    }
}

/*
* Function: execute_exit
* Description: Exits the shell after killing background processes
* Parameters: None
* Return: None
*/
void execute_exit()
{
    // exit
// The exit command exits your shell. It takes no arguments. When this command is run, 
// your shell must kill any other processes or jobs that your shell has started before it terminates itself.
    kill_background();  // Kill all background processes
    exit(0);            // Exit shell
}

/*
* Function: execute_cd
* Description: Changes the current directory
* Parameters: None
* Return: None
*/
void execute_cd()
{
    // cd
// The cd command changes the working directory of smallsh.

// By itself - with no arguments - it changes to the directory specified in the HOME environment variable
// This is typically not the location where smallsh was executed from, 
// unless your shell executable is located in the HOME directory, in which case these are the same.
// This command can also take one argument: the path of a directory to change to. 
// Your cd command must support both absolute and relative paths.

    chdir(argument_count == 1 ? getenv("HOME") : arguments[1]);  // Change to HOME or arg
}

/*
* Function: execute_status
* Description: Prints status of last foreground process
* Parameters: None
* Return: None
*/
void execute_status()
{
    //status
// The status command prints out either the exit status or the terminating signal of the last foreground process ran 
// by your shell.
// If this command is run before any foreground command is run, then it must simply return the exit status 0.
// The three built-in shell commands do not count as foreground processes for the purposes 
// of this built-in command - i.e., status must ignore built-in commands.
    if (WIFEXITED(last_status))
    {
        printf("exit value %d\n", WEXITSTATUS(last_status));  // Show exit value
    }
    else if (WIFSIGNALED(last_status))
    {
        printf("terminated by signal %d\n", WTERMSIG(last_status));  // Show signal
    }
    fflush(stdout);
        /*
        Be sure you flush out the output buffers each time you print, 
        as the text that you're outputting may not reach the screen until you do in this kind of interactive program. 
        To do this, call fflush() immediately after each and every time you output text.
        */
}

/*
* Function: run_command
* Description: Runs the parsed command
* Parameters: None
* Return: None
*/
void run_command()
{
    if (!strcmp(arguments[0], "exit"))
    {
        execute_exit();  // Exit command
    }
    else if (!strcmp(arguments[0], "cd"))
    {
        execute_cd();  // CD command
    }
    else if (!strcmp(arguments[0], "status"))
    {
        execute_status();  // Status command
    }
    else
    {
        execute_command();  // other commands
    }
}

/*
* Function: main_loop
* Description: Runs the main loop
* Parameters: None
* Return: None
*/
void main_loop()
{
    while (1)  // Loop
    {
        check_background();  // Check background processes
        show_prompt();       // Show prompt
        get_input();         // Get user input

        if (!is_valid_input())  // Skip if user input is invalid
        {
            continue;  // Go to next input
        }
        parse_input();  // Parse the command

        if (foreground_only_mode)
        {
            run_in_background = false;  // Force foreground
        }

        if (argument_count == 0)  // No args
        {
            continue;  // Next loop
        }
        run_command();  // Run it
    }
}

/*
* Function: main
* Description: Main for the program
* Parameters: None
* Return: 0
*/
int main()
{
    setup_signals();  // Set up signals
    main_loop();      // Start main loop
    return 0;        
}

/*
references:
https://www.geeksforgeeks.org/write-a-function-to-delete-a-linked-list/
https://www.geeksforgeeks.org/relational-database-from-csv-files-in-c/
https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
https://www.geeksforgeeks.org/generating-random-number-range-c/
https://www.geeksforgeeks.org/linked-list-in-c/
https://www.programiz.com/dsa/linked-list
https://www.geeksforgeeks.org/memmove-in-cc/
https://man7.org/linux/man-pages/man3/strtok_r.3.html
https://man7.org/linux/man-pages/man3/strtod.3p.html
https://man7.org/linux/man-pages/man3/random.3.html
https://www.man7.org/linux/man-pages/man3/sprintf.3.html
https://man7.org/linux/man-pages/man3/atoi.3.html
https://stackoverflow.com/questions/71152338/find-the-largest-and-smallest-file-with-one-loop-in-c
https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
https://www.geeksforgeeks.org/making-linux-shell-c/
https://www.geeksforgeeks.org/signals-c-language/
https://www.geeksforgeeks.org/conditional-or-ternary-operator-in-c/
https://www.tutorialspoint.com/c_standard_library/c_function_fflush.htm
R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces. Madison, WI, USA: Arpaci-Dusseau Books, 2018.
M. Kerrisk, The Linux Programming Interface: A Linux and UNIX System Programming Handbook. San Francisco, CA, USA: No Starch Press, 2010.
B. W. Kernighan and D. M. Ritchie, The C Programming Language, 2nd ed. Englewood Cliffs, NJ: Prentice Hall, 1988.
*/