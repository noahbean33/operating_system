#include "lib.h"

int main(void)
{
    while (1) {
        printf("process2\n");
        sleepu(100);
    }
    return 0;
}