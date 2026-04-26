// Simple implementation of the 'cat' command
// Concatenates and displays file contents to stdout
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    // Array to hold file descriptors for all input files
    FILE *fd[argc];
    
    // If no arguments provided, read from stdin
    if (argc <= 1)
    {
        fd[0] = stdin;
        int c;
        // Read character by character from stdin until EOF
        while((c = fgetc(fd[0])) != EOF)
        {
            putc(c, stdout);
        }
    }
    else
    {
        // Open all files specified in arguments
        for(int i=0; i<argc-1; i++)
        {
            fd[i] = fopen(argv[i+1], "r");
            // Error handling: exit if file cannot be opened
            if(fd[i] == NULL)
            {
                printf("Invalid file provided at index i=%d\n", i+1);
                exit(-1);
            }
        }
        int c;

        // Read and output each file sequentially
        for(int i=0; i<argc-1; i++)
        {
            // Read character by character until end of file
            while((c = fgetc(fd[i])) != EOF)
            {
                putc(c, stdout);
            }
        }
    }
    
    return 0;
}
