#include "lib.h"

int main(void)
{
    int pid;

    pid = fork();
    if (pid == 0) {
        printf("this is new process\n");
    }
    else {
        printf("this is the current process\n");
        waitu(pid);
    }
    
    return 0;
}