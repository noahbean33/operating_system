.section .data
    prompt: .asciz "$ "    @ The null-terminated string for the shell prompt.
    buffer: .space 256     @ A 256-byte buffer to store user input.
    argv_ptr: .word buffer, 0 @ An array of pointers for execve's argv.
                             @ It holds a pointer to the command string (buffer)
                             @ and is terminated by a null pointer (0).

.section .text
.global _start

_start:
    bl main                 @ Branch to the main function.

@ Main loop of the shell.
main:
    bl display_prompt       @ Display the "$ " prompt.
    bl read_input           @ Read a line of input from the user.
    bl execute_command      @ Fork and execute the command.
    b main                  @ Loop back to the start.

@ Displays the prompt string to standard output.
display_prompt:
    push {r4-r11, lr}       @ Save callee-saved registers and the link register.
    ldr r0, =prompt         @ Load the address of the prompt string into r0.
    bl print_string         @ Call the function to print the string.
    pop {r4-r11, pc}        @ Restore registers and return.

@ Prints a null-terminated string to standard output.
@ r0: Address of the string to print.
print_string:
    push {r4-r11, lr}       @ Save registers.
    mov r1, r0              @ Syscall 'write' needs the string address in r1.
    mov r2, #0              @ Use r2 as a counter to find the string length.

_print_loop:
    ldrb r3, [r1, r2]       @ Load a byte from the string into r3.
    cmp r3, #0              @ Check if it's the null terminator.
    beq _end_print          @ If it is, we've found the end.
    add r2, r2, #1          @ Otherwise, increment the length counter.
    b _print_loop           @ And loop again.

_end_print:
    @ Now r2 holds the length of the string.
    mov r7, #4              @ Syscall number for 'write' (4).
    mov r0, #1              @ File descriptor for stdout (1).
    @ r1 already contains the string address.
    @ r2 already contains the string length.
    svc #0                  @ Execute the system call.
    pop {r4-r11, pc}        @ Restore registers and return.

@ Reads a line of user input from stdin into the buffer.
read_input:
    push {r4-r11, lr}       @ Save registers.
    ldr r1, =buffer         @ Load buffer address into r1 for 'read' syscall.
    mov r2, #256            @ Set max bytes to read into r2.
    mov r7, #3              @ Syscall number for 'read' (3).
    mov r0, #0              @ File descriptor for stdin (0).
    svc #0                  @ Execute the system call.
                            @ The number of bytes read is returned in r0.
    bl strip_input          @ Remove the trailing newline character.
    pop {r4-r11, pc}        @ Restore registers and return.

@ Finds and replaces the trailing newline (0xA) with a null terminator (0x0).
strip_input:
    push {r4-r11, lr}       @ Save registers.
    mov r2, #0              @ Initialize loop counter/index to 0.
    ldr r1, =buffer         @ Load the base address of the buffer.

_strip_loop:
    ldrb r3, [r1, r2]       @ Load a byte from buffer[r2].
    cmp r3, #0xa            @ Is it a newline character?
    beq _remove_newline     @ If yes, go replace it.
    cmp r3, #0x0            @ Is it a null character?
    beq _end_loop           @ If yes, we're done (no newline found).
    add r2, r2, #1          @ Increment index.
    b _strip_loop           @ Continue searching.

_remove_newline:
    mov r0, #0              @ Load a null byte into r0.
    strb r0, [r1, r2]       @ Store the null byte where the newline was.

_end_loop:
    pop {r4-r11, pc}        @ Restore registers and return.

@ Main command execution logic.
execute_command:
    push {r4-r11, lr}       @ Save registers.
    ldr r0, =buffer         @ Load buffer address to pass to parser.
    bl parse_command        @ Call the (stubbed) parser.
    cmp r0, #0              @ Check if the command is empty.
    beq end_execute         @ If empty, do nothing and return.

    bl fork_process         @ Fork the current process.
    cmp r0, #0              @ Check the return value of fork().
    beq child_process       @ If 0, we are the child process.
    b wait_for_child        @ Otherwise, we are the parent, so we wait.

child_process:              @ This code is only executed by the child.
    ldr r0, =buffer         @ r0: filename (path to the executable).
    ldr r1, =argv_ptr       @ r1: argv (pointer to the argument array).
    mov r2, #0              @ r2: envp (environment variables, NULL).
    mov r7, #11             @ Syscall number for 'execve' (11).
    svc #0                  @ Execute the command.
    
    @ If execve returns, it means an error occurred.
    mov r7, #1              @ Syscall number for 'exit' (1).
    svc #0                  @ Exit the child process to prevent it from continuing.

wait_for_child:             @ This code is only executed by the parent.
    mov r7, #114            @ Syscall number for 'wait4' (114).
    mov r0, #-1             @ pid = -1 (wait for any child).
    mov r1, #0              @ status = NULL (don't store exit status).
    mov r2, #0              @ options = 0.
    svc #0                  @ Execute the system call.

end_execute:
    pop {r4-r11, pc}        @ Restore registers and return to the main loop.

@ A placeholder function for parsing commands and arguments.
@ In a real shell, this would split the buffer into tokens.
parse_command:
    push {lr}               @ Save link register.
    ldrb r1, [r0]           @ Load the first character of the buffer.
    cmp r1, #0              @ Check if the first character is null.
    movne r0, #1            @ If not, set r0 to 1 (non-empty command).
    moveq r0, #0            @ If it is, set r0 to 0 (empty command).
    pop {pc}                @ Return.

@ Creates a child process.
fork_process:
    push {r4-r11, lr}       @ Save registers.
    mov r7, #2              @ Syscall number for 'fork' (2).
    svc #0                  @ Execute the system call.
                            @ Returns PID in r0 for parent, 0 for child.
    pop {r4-r11, pc}        @ Restore registers and return.