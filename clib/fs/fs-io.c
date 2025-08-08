/*******************************************************************
 * File:        fs-io
 * Purpose:     Filesystem I/O dispatchers
 * Author:      Gerph
 * Date:        4 Aug 2025
 ******************************************************************/

#include <stdlib.h>

#include "kernel.h"
#include "swis.h"

#include "io/io-internal.h"
#include "fs/fs-errors.h"

static _io_dispatch_t __fs_dispatch;


static int __fs_close(FILE *fh)
{
    _kernel_osfind(0, (const char *)fh->_fileno); /* Close file */
    fh->_fileno = 0;
    if (fh->_flags & _IO_DELETEONCLOSE)
    {
        /* Delete the file */
        if (fh->_markers)
        {
            _kernel_osfile(6, fh->_markers->filename, NULL);
            free(fh->_markers);
            fh->_markers = NULL;
        }
    }
    return 0;
}


static int __fs_flush(FILE *fh)
{
    /* FIXME: Could call the flush OS_Args call */
    return 0;
}

static int __fs_check_eof(FILE *fh)
{
    int32_t at_eof;
    _kernel_oserror *err;
    err = _swix(OS_Args, _INR(0, 1)|_OUT(2), 5, fh->_fileno, &at_eof);
    if (err)
    {
        return -__fs_converterr(err);
    }
    return at_eof ? 1 : 0;
}


static int __fs_read_multiple(FILE *fh, void *ptr, size_t transfer)
{
    size_t not_transferred = 0;
    _kernel_oserror *err;
    err = _swix(OS_GBPB, _INR(0, 3)|_OUT(3), 4, fh->_fileno, ptr, transfer, &not_transferred);
    if (err)
    {
        return -__fs_converterr(err);
    }
    return transfer - not_transferred;
}

static int __fs_read_byte(FILE *fh)
{
    _kernel_oserror *err;
    int c;
    c = _kernel_osbget(fh->_fileno);
    if (c == -1)
        return IO_DISPATCH_EOF;
    return c;
}

static int __fs_write_multiple(FILE *fh, const void *ptr, size_t transfer)
{
    _kernel_oserror *err;
    size_t not_transferred = 0;
    err = _swix(OS_GBPB, _INR(0, 3)|_OUT(3), 2, fh->_fileno, ptr, transfer, &not_transferred);
    if (err)
    {
        return -__fs_converterr(err);
    }
    return transfer - not_transferred;
}

static int __fs_write_byte(FILE *fh, int c)
{
    _kernel_oserror *err;
    err = _swix(OS_BPut, _INR(0, 1), c, fh->_fileno);
    if (err)
    {
        return -__fs_converterr(err);
    }
    return 0;
}


static long int __fs_write_pos(FILE *fh, long int pos, int whence)
{
    _kernel_oserror *err;

    switch (whence)
    {
        case SEEK_SET:
            break;

        case SEEK_CUR:
            size_t cur = 0;
            err = _swix(OS_Args, _INR(0, 1)|_OUT(2), 0, fh->_fileno, &cur);
            if (err)
            {
                return -__fs_converterr(err);
            }
            pos += cur;
            break;

        case SEEK_END:
            size_t ext = 0;
            err = _swix(OS_Args, _INR(0, 1)|_OUT(2), 2, fh->_fileno, &ext);
            if (err)
            {
                return -__fs_converterr(err);
            }
            pos += ext;
            break;
    }
    err = _swix(OS_Args, _INR(0, 2), 1, fh->_fileno, pos);
    if (err)
    {
        return -__fs_converterr(err);
    }
    return pos;
}


static long int __fs_read_pos(FILE *fh)
{
    _kernel_oserror *err;

    int32_t cur = 0;
    err = _swix(OS_Args, _INR(0, 1)|_OUT(2), 0, fh->_fileno, &cur);
    if (err)
    {
        return -__fs_converterr(err);
    }
    return cur;
}



static int __fs_read_line(FILE *fh, char *str, size_t size)
{
    char *p;
    for (p = str; p - str < size - 1;)
    {
        int c = __fs_read_byte(fh);
        if (c < 0)
            return c;
        *p++ = c;
        if (c = '\n')
            break;
    }
    *p = '\0';
    return 0;
}


/*************************************************** Gerph *********
 Function:      __fs_setup_dispatch
 Description:   Set up the dispatch table used by the I/O system
 Parameters:    fh-> the file handle
 Returns:       none
 ******************************************************************/
void __fs_setup_dispatch(FILE *fh)
{
    if (__fs_dispatch.close == NULL)
    {
        __fs_dispatch.read_multiple = __fs_read_multiple;
        __fs_dispatch.read_byte = __fs_read_byte;
        __fs_dispatch.read_line = __fs_read_line;
        __fs_dispatch.write_multiple = __fs_write_multiple;
        __fs_dispatch.write_byte = __fs_write_byte;
        __fs_dispatch.close = __fs_close;
        __fs_dispatch.flush = __fs_flush;
        __fs_dispatch.read_pos = __fs_read_pos;
        __fs_dispatch.write_pos = __fs_write_pos;
        __fs_dispatch.check_eof = __fs_check_eof;
    }

    fh->_IO_save_base = (void*)&__fs_dispatch;
}
