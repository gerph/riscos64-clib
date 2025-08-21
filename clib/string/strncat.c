#include <stddef.h>
#include <string.h>

char *strncat(char *a, const char *b, size_t n)
{
    char *oa = a;
    /* Find the end of the string */
    while (*a++)
        ;
    a--;
    while (--n >= 0)
    {
        char c = *b++;
        *a++ = c;
        if (c=='\0')
            break;
    }
    return oa;
}
