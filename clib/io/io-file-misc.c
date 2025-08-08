#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"
#include "swis_os.h"
#include "io/io-internal.h"
#include "fs/fs-errors.h"
#include <errno.h>


int feof(FILE *fh)
{
    int result;

    CHECK_MAGIC(fh, -1);

    if (IO_AT_EOF(fh))
        return 1;

    result = IO_DISPATCH(fh)->check_eof(fh);
    if (result < 0)
    {
        errno = -result; /* FIXME: Apparently these functions should not fail or set errno? */
        return 1;
    }
    return result;
}

int ferror(FILE *fh)
{
    /* FIXME: We don't support error flagging at the moment */
    return 0;
}


int fileno(FILE *fh)
{
    CHECK_MAGIC(fh, -1);

    if (IO_IS_CONSOLE(fh))
    {
        errno = EBADF;
        return IO_FD_CONSOLE;
    }

    return fh->_fileno;
}


void setbuf(FILE *fh, char *buf)
{
    /* FIXME: We don't support buffering at the moment */
    return;
}

int setvbuf(FILE *fh, char *buf, int type, size_t size)
{
    /* FIXME: We don't support buffering at the moment */
    return 0;
}

void clearerr(FILE *fh)
{
    /* FIXME: We don't support error flagging at the moment */
    return;
}

int isatty(int fd)
{
    if (fd == IO_FD_CONSOLE)
        return 1;
    return 0;
}
