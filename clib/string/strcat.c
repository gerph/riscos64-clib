#include <stddef.h>
#include <string.h>

char *strcat(char *a, const char *b)
{
    char *oa = a;
    /* Find the end of the string */
    while (*a++)
        ;
    a--;
    while (1)
    {
        char c = *b++;
        *a++ = c;
        if (c=='\0')
            break;
    }
    return oa;
}
