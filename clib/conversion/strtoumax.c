#include <stdint.h>
#include <string.h>
#include <stdlib.h>

uintmax_t strtoumax(const char *str, char **endptr, int base)
{
    return strtoull(str, endptr, base);
}
