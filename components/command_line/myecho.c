#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* nlopt = "-n";
int main(int argc, const char* argv[])
{
    if (argc == 1)
    {
        printf("\n");
        exit(0);
    }
    if (argc == 2 && strcmp(argv[1], nlopt))
    {
        exit(0);
    }
    int enable_newline = strcmp(argv[1], nlopt);
    for(int i=enable_newline ? 1:2; i<argc; i++)
    {
        printf("%s", argv[i]);
        if (i != argc-1)
           printf(" "); 
    }
    if (enable_newline)
        printf("\n");
}
