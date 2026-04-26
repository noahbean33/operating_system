// Simple implementation of the 'echo' command
// Prints arguments to stdout with optional newline suppression
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Option flag to suppress trailing newline
const char* nlopt = "-n";
int main(int argc, const char* argv[])
{
    // If no arguments provided, just print a newline and exit
    if (argc == 1)
    {
        printf("\n");
        exit(0);
    }
    // If only argument is "-n" flag, print nothing and exit
    if (argc == 2 && strcmp(argv[1], nlopt))
    {
        exit(0);
    }
    // Check if first argument is "-n" flag (0 = match, non-zero = different)
    int enable_newline = strcmp(argv[1], nlopt);
    // Start from index 1 if newline enabled, 2 if "-n" flag present
    for(int i=enable_newline ? 1:2; i<argc; i++)
    {
        printf("%s", argv[i]);
        // Add space between arguments (but not after last one)
        if (i != argc-1)
           printf(" "); 
    }
    // Print trailing newline unless "-n" flag was provided
    if (enable_newline)
        printf("\n");
}
