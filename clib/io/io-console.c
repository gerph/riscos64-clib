/*******************************************************************
 * File:        io-console
 * Purpose:     Simple console input/output functions
 * Author:      Gerph
 * Date:        10 Aug 2024
 ******************************************************************/

#include <stdlib.h>
#include <string.h>
#include "swis_os.h"

#include "io-internal.h"

int getchar(void)
{
    return getc(stdin);
}

int putchar(int c)
{
    return putc(c, stdout);
}

int puts(const char *ptr)
{
    int len = fputs(ptr, stdout);
    fputc('\n', stdout);
    return len + 1;
}
