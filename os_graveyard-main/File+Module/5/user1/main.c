#include "lib.h"

int main(void)
{
    int fd;
    int size;
    char buffer[100] = { 0 };

    fd = open_file("TEST.BIN");

    if (fd == -1) {
        printf("open file failed");
    }
    else {
        size = get_file_size(fd);
        size = read_file(fd, buffer, size);

        if (size != -1) {
            printf("%s\n", buffer);
            printf("read %db in total", size);
        }   
    }

    while (1) {}
    
    return 0;
}