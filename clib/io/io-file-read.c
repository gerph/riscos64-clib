#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"
#include "swis_os.h"
#include "io/io-internal.h"
#include "fs/fs-errors.h"
#include <errno.h>


size_t fread(void *ptr, size_t size, size_t nitems, FILE *fh)
{
    CHECK_MAGIC(fh, 0);

    if (!IO_IS_READABLE(fh))
    {
        errno = EPERM;
        return 0;
    }

    if (IO_AT_EOF(fh))
    {
        return 0;
    }

    /* Shortcut reading a single byte if we can */
    if (size == 1 && nitems == 1)
    {
        char *p = (char*)ptr;
        int c = fgetc(fh);
        if (c == EOF)
            return 0;
        *p = c;
        return 1;
    }

    size_t transfer = size * nitems;
    int transferred = 0;
    size_t totransfer = transfer;

    if (transfer == 0)
        return 0; /* Nothing to do?! */

    if (fh->_flags & _IO_CHARPUSHED)
    {
        uint8_t *data = (uint8_t *)ptr;
        *data++ = fh->_shortbuf[0];
        /* Unmark as any character pushed back */
        fh->_flags &= ~ _IO_CHARPUSHED;
        totransfer -= 1;
        ptr = (void*)data;
    }

    if (totransfer != 0)
    {
        transferred = IO_DISPATCH(fh)->read_multiple(fh, ptr, totransfer);
        if (transferred < 0)
        {
            errno = -transferred;
            return 0;
        }
    }

    if (transferred == totransfer)
        return nitems;

    if (size == 1)
        return transferred;

    /* They requested a size that wasn't the whole, so we need to move back */
    {
        nitems = transferred / nitems;
        size_t excess = transferred % nitems;
        if (excess)
        {
            /* Move back the amount we didn't read */
            fseek(fh, -excess, SEEK_CUR);
        }

        return nitems;
    }
}

int fgetc(FILE *fh)
{
    int32_t c;

    CHECK_MAGIC(fh, EOF);

    if (!IO_IS_READABLE(fh))
    {
        errno = EPERM;
        return EOF;
    }
    if (IO_AT_EOF(fh))
    {
        return EOF;
    }

    if (fh->_flags & _IO_CHARPUSHED)
    {
        c = fh->_shortbuf[0];
        /* Unmark as any character pushed back */
        fh->_flags &= ~ _IO_CHARPUSHED;
        return c;
    }

    c = IO_DISPATCH(fh)->read_byte(fh);
    if (c == IO_DISPATCH_EOF)
    {
        fh->_flags |= _IO_EOF;
        return 0;
    }
    if (c < 0)
    {
        errno = -c;
        c = EOF;
    }
    return c;
}


int getc(FILE *fh)
{
    return fgetc(fh);
}

int ungetc(int c, FILE *fh)
{
    CHECK_MAGIC(fh, -1);

    if (!IO_IS_READABLE(fh))
    {
        errno = EPERM;
        return EOF;
    }

    if (fh->_flags & _IO_CHARPUSHED)
        return EOF; /* Already pushed so cannot push more */
    if (c == EOF)
        return EOF; /* No push if they tried pushing EOF */
    fh->_flags |= _IO_CHARPUSHED;
    fh->_flags &= ~_IO_EOF; /* Clear the EOF state */
    fh->_shortbuf[0] = c;
    return c;
}
