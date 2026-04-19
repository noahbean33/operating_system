#include "lib.h"
#include "stdint.h"

int main(void)
{
    int total = get_total_memoryu();
    printf("total memory is %dmb\n", (int64_t)total);
    
    return 0;
}

