#include <stdarg.h>
#include <string.h>
#include "io-vprintf.h"
#include "io-internal.h"

typedef struct outputterstr_t {
    _io_outputter_t out;
    char *start;
    char *p;
    int size;
    int left;
} outputterstr_t;

static int str_writen(outputter_t *out, const char *str, size_t len)
{
    outputterstr_t *outs = (outputterstr_t *)out;
    //printf("str_writen : writing %p (len %i) to %p (left = %i)\n", str, len, outs->p, outs->left);
    if (outs->left < (int)len)
    {
        /* This would exceed the buffer size */
        if (outs->left <= 0)
        {
            /* It's wholey outside the buffer */
            outs->p += len;
        }
        else
        {
            /* Partially fills the buffer */
            memcpy(outs->p, str, outs->left);
            outs->p[outs->left] = '\0';
            outs->p += len;
            outs->left = 0;
        }
    }
    else
    {
        memcpy(outs->p, str, len);
        outs->p += len;
        *outs->p = '\0';
        outs->left -= len;
    }
    return len;
}

static int str_write0(outputter_t *out, const char *str)
{
    outputterstr_t *outs = (outputterstr_t *)out;
    int len = strlen(str);
    int wrote;
    wrote = str_writen(out, str, len);
    return wrote;
}

static int str_writec(outputter_t *out, char c)
{
    outputterstr_t *outs = (outputterstr_t *)out;
    if (outs->p + 1 + 1 - outs->start > outs->size)
    {
        /* This would exceed the buffer size */
        outs->p++;
    }
    else
    {
        *outs->p++ = c;
        *outs->p = '\0';
    }
    return 1;
}

static int str_newline(outputter_t *out)
{
    return str_writec(out, 10);
}

int vsprintf(char *str, const char *format, va_list args)
{
    int n;
    outputterstr_t out;
    out.out.write0 = str_write0;
    out.out.writen = str_writen;
    out.out.writec = str_writec;
    out.out.newline = str_newline;
    out.start = str;
    out.p = str;
    out.size = (str == NULL) ? 0 : 0x7FFFFFFF;
    out.left = out.size;
    //printf("sprintf(%x, '%s', ...)\n", out.start, format);
    n = _vprintf((outputter_t*)&out, format, args);
    if (n==0)
    {
        if (out.size > 0)
            *out.p = '\0';
    }
    return n;
}

int sprintf(char *str, const char *format, ...)
{
    int n;
    va_list args;
    va_start(args, format);

    n = vsprintf(str, format, args);

    va_end(args);
    return n;
}

int vsnprintf(char *str, size_t max, const char *format, va_list args)
{
    int n;
    outputterstr_t out;
    out.out.write0 = str_write0;
    out.out.writen = str_writen;
    out.out.writec = str_writec;
    out.out.newline = str_newline;
    out.start = str;
    out.p = str;
    out.size = max - 1;
    out.left = out.size;
    //printf("sprintf(%x, '%s', ...)\n", out.start, format);
    n = _vprintf((outputter_t*)&out, format, args);
    return n;
}

int snprintf(char *str, size_t max, const char *format, ...)
{
    int n;
    va_list args;
    va_start(args, format);

    n = vsnprintf(str, max, format, args);

    va_end(args);
    return n;
}
