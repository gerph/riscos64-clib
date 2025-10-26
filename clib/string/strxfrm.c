#include <stddef.h>
#include <string.h>

/* FIXME: Does not take account of the locale */
size_t strxfrm(char *dst, const char *src, size_t n)
{
    char *odst = dst;
    if (n == 0)
        return 0;
    n -= 1;
    while (n-->0)
    {
        int c = *src++;
        *dst++ = c;
        if (!c)
            break;
    }
    *dst++ = '\0';
    return dst - odst;
}
