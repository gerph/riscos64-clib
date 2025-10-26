#include <stddef.h>
#include <string.h>

/* FIXME: Does not take account of the locale */
int strcoll(const char *a, const char *b)
{
    while (1)
    {
        int ac = *a++;
        int bc = *b++;
        if (ac != bc)
        {
            return ac - bc;
        }
        if (ac == 0)
            return 0;
    }
}
