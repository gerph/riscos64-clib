#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"
#include "swis_os.h"
#include "io/io-internal.h"
#include "fs/fs-errors.h"
#include <errno.h>


int fseek(FILE *fh, long int pos, int whence)
{
    int result;

    CHECK_MAGIC(fh, -1);

    if (IO_IS_CONSOLE(fh))
    {
        errno = EPERM;
        return -1;
    }

    if (IO_IS_APPEND(fh))
    {
        errno = EINVAL; /* You tried to seen on an append file handle... you can't do that */
        return -1;
    }

    result = IO_DISPATCH(fh)->write_pos(fh, pos, whence);
    if (result < 0)
    {
        errno = -result;
        return -1;
    }
    else
    {
        /* No error, so unmark as any character pushed back and EOF */
        fh->_flags &= ~ (_IO_CHARPUSHED | _IO_EOF);
    }
    return 0;
}


long int ftell(FILE *fh)
{
    long int pos;

    CHECK_MAGIC(fh, -1);

    if (IO_IS_CONSOLE(fh))
    {
        errno = EPERM;
        return -1;
    }

    pos = IO_DISPATCH(fh)->read_pos(fh);
    if (pos < 0)
    {
        errno = -pos;
        return -1;
    }

    return pos;
}
