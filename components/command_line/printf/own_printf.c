// Custom implementations of string printing functions
// Demonstrates different ways to print strings without using printf directly
#include <stdio.h>
#include <stdarg.h>

// Method 1: Print string using fputs (simplest approach)
void printstr(const char *str)
{
    // Uses standard library function that writes string to file stream
    fputs(str, stdout); 
}

// Method 2: Print string character-by-character using pointer arithmetic
void printstr_chars(const char *str)
{
    int i=0;
    // Iterate through string until null terminator
    while(*(str+i) != '\0')
    {
        // Print each character individually using fputc
        fputc(*(str+i++), stdout);
    }
}

// Method 3: Print formatted string with variable arguments (like printf)
void formatted_print(const char* format, ...)
{
    // Variable argument list to hold dynamic parameters
    va_list list;
    // Initialize va_list starting after 'format' parameter
    va_start(list, format);
    // Use vfprintf to handle format string and variable arguments
    vfprintf(stdout, format, list);
    // Clean up variable argument list
    va_end(list);
}

int main()
{
    // Test string with format specifiers
    const char *str = "A line of text with %d placeholders: %s\n";

    // Demonstrate method 1: prints literal string (no formatting)
    printstr(str);
    // Demonstrate method 2: prints character by character (no formatting)
    printstr_chars(str);
    // Demonstrate method 3: prints with format substitution
    formatted_print(str, 2, "elephant");
}
