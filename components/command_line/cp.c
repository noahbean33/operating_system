#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define MEM_SIZE 64

int main(int argc, char *argv[])
{
    if ( argc == 1 )
    {
        printf("%s: missing file operand\n", argv[0]);
    } 
    if ( argc == 2 )
    {
        printf("%s: missing destination file operand after '%s'\n", argv[0], argv[1]);
        return 0;
    }

    char memory[MEM_SIZE];
    int n;
    struct stat stackstat;
    int source_fd;
    int target_fd;
    int dir_fd;

    if (argc == 3)
    {
        stat(argv[1], &stackstat);
        source_fd = open(argv[1], O_RDONLY, 0);
        target_fd = creat(argv[2], stackstat.st_mode);

        // single file-copy
        while ( (n = read(source_fd, memory, MEM_SIZE)) > 0 )
        {
            write(target_fd, memory, n);
        }
    }
    else
    {
        // multiple source file args: target is directory
        for (int i=1; i<argc-1; i++)
        {
            source_fd = open(argv[i], O_RDONLY, 0);
            dir_fd = open(argv[argc-1], O_RDONLY, 0);
            stat(argv[1], &stackstat);
            target_fd = openat(dir_fd, argv[i], O_CREAT|O_WRONLY|O_TRUNC, stackstat.st_mode);

            while ( (n = read(source_fd, memory, MEM_SIZE)) > 0 )
            {
                write(target_fd, memory, n);
            }
        }
    }
}
