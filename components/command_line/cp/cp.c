// Simple implementation of the 'cp' command
// Copies files using low-level file I/O operations
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Buffer size for reading/writing file data
#define MEM_SIZE 64

int main(int argc, char *argv[])
{
    // Error checking: ensure proper usage with source and destination arguments
    if ( argc == 1 )
    {
        printf("%s: missing file operand\n", argv[0]);
    } 
    if ( argc == 2 )
    {
        printf("%s: missing destination file operand after '%s'\n", argv[0], argv[1]);
        return 0;
    }

    // Buffer for reading file data in chunks
    char memory[MEM_SIZE];
    // Number of bytes read in each iteration
    int n;
    // Structure to hold file metadata (permissions, etc.)
    struct stat stackstat;
    // File descriptors for source, target, and directory
    int source_fd;
    int target_fd;
    int dir_fd;

    // Case 1: Single file copy (source -> destination)
    if (argc == 3)
    {
        // Get source file metadata to preserve permissions
        stat(argv[1], &stackstat);
        // Open source file for reading
        source_fd = open(argv[1], O_RDONLY, 0);
        // Create target file with same permissions as source
        target_fd = creat(argv[2], stackstat.st_mode);

        // Copy file data in chunks
        while ( (n = read(source_fd, memory, MEM_SIZE)) > 0 )
        {
            write(target_fd, memory, n);
        }
    }
    // Case 2: Multiple source files -> directory
    else
    {
        // Last argument is the target directory
        // Loop through all source files
        for (int i=1; i<argc-1; i++)
        {
            // Open current source file for reading
            source_fd = open(argv[i], O_RDONLY, 0);
            // Open target directory
            dir_fd = open(argv[argc-1], O_RDONLY, 0);
            // Get source file metadata
            stat(argv[1], &stackstat);
            // Create target file within directory with same name
            target_fd = openat(dir_fd, argv[i], O_CREAT|O_WRONLY|O_TRUNC, stackstat.st_mode);

            // Copy file data in chunks
            while ( (n = read(source_fd, memory, MEM_SIZE)) > 0 )
            {
                write(target_fd, memory, n);
            }
        }
    }
}
