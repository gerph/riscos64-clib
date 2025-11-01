#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"
#include "swis_os.h"
#include "io/io-internal.h"
#include "fs/fs-errors.h"
#include <errno.h>


size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *fh)
{
    CHECK_MAGIC(fh, 0);

    if (!IO_IS_WRITABLE(fh))
    {
        errno = EPERM;
        return 0;
    }
    if (size == 0 || nitems == 0)
        return 0;

    size_t totransfer = size * nitems;
    int transferred = IO_DISPATCH(fh)->write_multiple(fh, ptr, totransfer);
    if (transferred < 0)
    {
        errno = -transferred;
        return 0;
    }
    else
    {
        /* No error, so unmark as any character pushed back */
        fh->_flags &= ~ _IO_CHARPUSHED;
    }

    if (transferred == totransfer)
        return nitems;

    if (size == 1)
        return transferred;

    /* They tried to write things, but didn't write a whole record... we may have corrupted data, but let's report */
    {
        nitems = transferred / nitems;
        size_t excess = transferred % nitems;
        if (excess)
        {
            /* Move back the amount we wrote which wasn't a whole item */
            fseek(fh, -excess, SEEK_CUR);
        }

        return nitems;
    }
}

int fflush(FILE *fh)
{
    CHECK_MAGIC(fh, -1);

    if (!IO_IS_WRITABLE(fh))
    {
        errno = EPERM;
        return 0;
    }

    if (IO_IS_CONSOLE(fh))
        return 0;

    int result = IO_DISPATCH(fh)->flush(fh);
    if (result < 0)
    {
        errno = -result;
        return -1;
    }

    return 0;
}


int fputc(int c, FILE *fh)
{
    CHECK_MAGIC(fh, -1);

    if (!IO_IS_WRITABLE(fh))
    {
        errno = EPERM;
        return 0;
    }

    c = IO_DISPATCH(fh)->write_byte(fh, c);
    if (c < 0)
    {
        errno = -c;
        c = EOF;
    }
    else
    {
        /* No error, so unmark as any character pushed back */
        fh->_flags &= ~ _IO_CHARPUSHED;
    }
    return c;
}

int putc(int c, FILE *fh)
{
    return fputc(c, fh);
}

int fputs(const char *str, FILE *fh)
{
    if (str == NULL)
        str = "<NULL>";

    int len = strlen(str);
    int wrote;

    wrote = fwrite(str, 1, len, fh);
    wrote += 1;
    // fputs does NOT write a newline (puts does)
    return wrote;
}
