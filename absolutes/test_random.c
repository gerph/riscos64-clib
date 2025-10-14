#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MAXNUMS 10

int main(int argc, char *argv[])
{
    uint32_t nums[MAXNUMS];
    int i;
    for (i=0; i<MAXNUMS; i++)
    {
        uint32_t r = rand();
        int o;
        printf("Random %2i: %10u\n", i, r);
        for (o=0; o<i; o++)
        {
            if (nums[o] == r)
            {
                printf("Repeats previous value (index %i)\n", o);
                return 1;
            }
        }
        nums[i] = r;
    }
    return 0;
}

