/*******************************************************************
 * File:        io-internal.h
 * Purpose:     Internal information for our IO implementation
 * Author:      Gerph
 * Date:        5 Mar 2025
 ******************************************************************/

#ifndef IO_INTERNAL_H
#define IO_INTERNAL_H

#include <errno.h>
#include <limits.h>
#include <stdio.h>

extern FILE *__file_list;

/* For fields, see:
     /usr/xcc/aarch64-unknown-linux-gnu/aarch64-unknown-linux-gnu/sysroot/usr/include/bits/types/struct_FILE.h

   We use (some fields as misnamed):
    * `_flags` - to mark our streams as valid and their capabilities.
    * `_shortbuf[0]` - for the character which was ungetc'd.
    * `_fileno` - for the RISC OS file handle that is used.
    * `_chain` - links together all the FILE blocks to allow them to be freed.
    * `_markers` - may contain the filename used by the file when 'delete on close' set.
    * `_IO_save_base` - points to the dispatchers for FILE operations
 */

/* Flags we use: */
#define _IO_CHARPUSHED      (1u<<0)  /* Character has been pushed back on to stream with ungetc */
#define _IO_DELETEONCLOSE   (1u<<1)  /* Stream must be deleted when closed (for tmpfile) */
#define _IO_READABLE        (1u<<2)  /* Can be read */
#define _IO_WRITABLE        (1u<<3)  /* Can be written */
#define _IO_CONSOLE         (1u<<4)  /* Is a console */
#define _IO_APPEND          (1u<<5)  /* Is append-only */
#define _IO_EOF             (1u<<6)  /* At EOF until seek */
#define _IO_BINARY          (1u<<7)  /* Opened as binary */

#define _IO_MAGIC (0x381F0000u)
#define _IO_MAGIC_MASK (0xFFFF0000u)
#define CHECK_MAGIC0(fh) \
                        do { \
                            if ( (fh) == NULL || (((fh)->_flags & _IO_MAGIC_MASK) != _IO_MAGIC) ) \
                            { \
                                errno = EBADF; \
                                return; \
                            } \
                        } while (0)
#define CHECK_MAGIC(fh, fail_code) \
                        do { \
                            if ( (fh) == NULL || (((fh)->_flags & _IO_MAGIC_MASK) != _IO_MAGIC) ) \
                            { \
                                errno = EBADF; \
                                return (fail_code); \
                            } \
                        } while (0)

/* All IO dispatcher functions return either a value for result, or -ERRNO */
typedef int (*_io_read_multiple)(FILE *fh, void *ptr, size_t size);
typedef int (*_io_read_byte)(FILE *fh);
typedef int (*_io_read_line)(FILE *fh, char *ptr, size_t size); /* read to \n */
typedef int (*_io_write_multiple)(FILE *fh, const void *ptr, size_t size);
typedef int (*_io_write_byte)(FILE *fh, int c);
typedef int (*_io_close)(FILE *fh);
typedef int (*_io_flush)(FILE *fh);
typedef long int (*_io_read_pos)(FILE *fh);
typedef long int (*_io_write_pos)(FILE *fh, long int pos, int whence);
typedef int (*_io_check_eof)(FILE *fh);


/* Functions used by the printf family of functions for writing to stream */
typedef struct _io_outputter_s {
    int (*write0)(struct _io_outputter_s *out, const char *str);
    int (*writec)(struct _io_outputter_s *out, char c);
    int (*writen)(struct _io_outputter_s *out, const char *str, size_t len);
    int (*newline)(struct _io_outputter_s *out);
    void *private;
} _io_outputter_t;


typedef struct _io_dispatch_s {
    _io_read_multiple   read_multiple;
    _io_read_byte       read_byte;
    _io_read_line       read_line;
    _io_write_multiple  write_multiple;
    _io_write_byte      write_byte;
    _io_close           close;
    _io_flush           flush;
    _io_read_pos        read_pos;
    _io_write_pos       write_pos;
    _io_check_eof       check_eof;
} _io_dispatch_t;

#define IO_DISPATCH_EOF (-256) /* Value returned by _io_read_multiple/line to indicate an EOF condition */

/* _IO_marker is used to give us information about the file - it doesn't have to be here, but it's easy to add */
struct _IO_marker {
    char filename[PATH_MAX]; /* temporary filename name */
};

#define IO_IS_READABLE(fh)  ((fh)->_flags & _IO_READABLE)
#define IO_IS_WRITABLE(fh)  ((fh)->_flags & _IO_WRITABLE)
#define IO_IS_APPEND(fh)    ((fh)->_flags & _IO_APPEND)

#define IO_IS_CONSOLE(fh)   ((fh)->_flags & _IO_CONSOLE)
#define IO_IS_KEYBOARD(fh)  (((fh)->_flags & (_IO_CONSOLE | _IO_READABLE)) == (_IO_CONSOLE | _IO_READABLE))
#define IO_IS_SCREEN(fh)    (((fh)->_flags & (_IO_CONSOLE | _IO_WRITABLE)) == (_IO_CONSOLE | _IO_WRITABLE))

#define IO_AT_EOF(fh)       ((fh)->_flags & _IO_EOF)

/* Get the pointer to the dispatch table for this FILE */
#define IO_DISPATCH(fh)     ( (_io_dispatch_t *)((fh)->_IO_save_base) )

/* fileno used for consoles */
#define IO_FD_CONSOLE       (-2)

#endif
