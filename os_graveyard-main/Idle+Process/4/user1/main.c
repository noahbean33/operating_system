#include "lib.h"

int main(void)
{
    int64_t i = 0;

    while (1) {
        printf("user process %d\n", i);
        sleepu(100);
        i++;
    }
    return 0;
}