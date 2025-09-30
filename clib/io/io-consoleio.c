/*******************************************************************
 * File:        io-consoleio
 * Purpose:     Console I/O dispatchers
 * Author:      Gerph
 * Date:        4 Aug 2025
 ******************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel.h"
#include "kernel/kernel-errors.h"

#include "swis.h"
#include "swis_os.h"

#include "io/io-internal.h"
#include "time/time-clock.h"

static _io_dispatch_t __con_dispatch;


static const struct {
    int32_t errnum;
    char errmess[8];
} err_Escape = { ErrorNumber_Escape, "Escape" };


/*************************************************** Gerph *********
 Function:      os_readline
 Description:   Call OS_ReadLine for input
 Parameters:    line-> line to read
                len = max length including terminator
 Returns:       pointer to error if failed, or NULL if ok
 ******************************************************************/
static _kernel_oserror *os_readline(char *line, int len)
{
    _kernel_oserror *err;
    int read;
    uint32_t flags = 0;
#ifdef __riscos64
    err = _swix(OS_ReadLine32, _INR(0, 4)|_OUTR(0, 1), line, len, 32, 128, 0, &flags, &read);
    if (err)
        return err;
    if (flags)
        goto escape;
    line[read] = '\0';
    return NULL;
#else
    err = _swix(OS_ReadLine32, _INR(0, 4)|_OUT(1)|_OUT(_FLAGS), line, len, 32, 128, 0, &read, &flags);
#ifndef ErrorNumber_ModuleBadSWI
#define ErrorNumber_ModuleBadSWI          0x110
#endif
    if (err && err->errnum == ErrorNumber_ModuleBadSWI)
    {
        err = _swix(OS_ReadLine, _INR(0, 4)|_OUT(1)|_OUT(_FLAGS), line, len, 32, 128, 0, &read, &flags);
    }
    if (err)
        return err;
    if (flags & _C)
        goto escape;
    line[read] = '\0';
    return NULL;
#endif

escape:
    return (_kernel_oserror *)&err_Escape;
}


static int __con_close(FILE *fh)
{
    /* Nothing to do */
    return 0;
}


static int __con_flush(FILE *fh)
{
    /* Nothing to do */
    return 0;
}

static int __con_check_eof(FILE *fh)
{
    if (IO_IS_SCREEN(fh))
        return 0;
    if (IO_IS_KEYBOARD(fh))
    {
        /* FIXME: Should check for input and if so look for ctrl-d or something ? */
        return 0;
    }

    return 0;
}


static int __con_read_multiple(FILE *fh, void *ptr, size_t transfer)
{
    /* FIXME: Currently you cannot read multiple input */
    return 0;
}

static int __con_read_byte(FILE *fh)
{
    /* Will never be called for output */
    /* FIXME: Should return failure if we had EOF? */
    __clock_freeze(true);
    int b = os_readc();
    if (! (fh->_flags & _IO_BINARY))
    {
        /* Non-binary input read */
        if (b == '\r')
            b = '\n';
        if (b == '\r')
            b = '\n';
        if (b == '\n')
        {
            os_newline();
        }
        else if (b == '\b' || b == '\x7f')
        {
            os_writec(0x7f); /* Delete character */
        }
        else if (b < ' ' && b != '\a') /* escapes for anything but BEL */
        {
            os_writec('|');
            os_writec(b + '@');
        }
        else
        {
            os_writec(b);
        }
    }
    __clock_freeze(false);
    return b;
}

static int __con_write_multiple(FILE *fh, const void *ptr, size_t transfer)
{
    _kernel_oserror *err;
    int wrote = 0;
    while (transfer)
    {
        const char *next_nl = memchr(ptr, '\n', transfer);
        if (next_nl == NULL)
        {
            err = os_writen(ptr, transfer);
            if (err)
                break;
            wrote += transfer;
            break;
        }
        int to_nl = (next_nl - (const char *)ptr);
        if (to_nl)
        {
            err = os_writen(ptr, to_nl);
            if (err)
                break;
            wrote += to_nl;
        }
        err = os_newline();
        if (err)
            break;
        wrote += 1;
        transfer -= to_nl + 1;
        ptr = ((const char *)ptr) + to_nl + 1;
    }
    return wrote;
}

static int __con_write_byte(FILE *fh, int c)
{
    /* Will never be called for input */
    if (c == '\n')
        os_newline();
    else
        os_writec(c);
    return c;
}


static long int __con_write_pos(FILE *fh, long int pos, int whence)
{
    return -EIO;
}


static long int __con_read_pos(FILE *fh)
{
    return 0;
}


static int __con_read_line(FILE *fh, char *str, size_t size)
{
    _kernel_oserror *err;
    int len;
    __clock_freeze(true);
    err = os_readline(str, size - 1);
    __clock_freeze(false);
    if (err)
    {
        return -EINTR;
    }
    /* No error, but the line should be returned with the newline present */
    len = strlen(str);
    str[len] = '\n';
    str[len + 1] = '\0';
    return 0;
}

/*************************************************** Gerph *********
 Function:      __con_setup_dispatch
 Description:   Set up the dispatch table used by the I/O system
 Parameters:    fh-> the file handle
 Returns:       none
 ******************************************************************/
void __con_setup_dispatch(FILE *fh)
{
    if (__con_dispatch.close == NULL)
    {
        __con_dispatch.read_multiple = __con_read_multiple;
        __con_dispatch.read_byte = __con_read_byte;
        __con_dispatch.read_line = __con_read_line;
        __con_dispatch.write_multiple = __con_write_multiple;
        __con_dispatch.write_byte = __con_write_byte;
        __con_dispatch.close = __con_close;
        __con_dispatch.flush = __con_flush;
        __con_dispatch.read_pos = __con_read_pos;
        __con_dispatch.write_pos = __con_write_pos;
        __con_dispatch.check_eof = __con_check_eof;
    }

    fh->_IO_save_base = (void*)&__con_dispatch;
}
