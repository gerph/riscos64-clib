#include <stdint.h>
#include <string.h>
#include <stdlib.h>

uintmax_t strtoimax(const char *str, char **endptr, int base)
{
    return strtoll(str, endptr, base);
}
