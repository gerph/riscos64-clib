/* Very very simple sscanf */

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>


typedef struct formatparams_s {
    int field_width;
    bool discard;
    int param_width;
} formatparams_t;


int sscanf(const char *str, const char *format, ...)
{
    int n;
    va_list args;
    va_start(args, format);

    n = vsscanf(str, format, args);

    va_end(args);
    return n;
}



int vsscanf(const char *str, const char *format, va_list args)
{
    int converted = 0;
    const char *start = str;
    //printf("vsscanf '%s' with '%s'\n", str, format);
    while (*format)
    {
        formatparams_t params = {0};
        char c = *format++;
        switch (c)
        {
            case ' ':
                /* Space matches zero or more whitespace characters */
                while (*str && isspace(*str))
                    str++;
                break;

            case '%':
                /* A format conversion */
                /* FIXME: Flag characters */
                /* FIXME: Field width */

                c = *format++;
                if (c == '\0')
                    goto done;

                if (c>='1' && c<='9')
                {
                    /* Field width */
                    params.field_width = c - '0';
                    c = *format++;
                    while (c>='0' && c<='9')
                    {
                        params.field_width = (params.field_width * 10) + c - '0';
                        c = *format++;
                    }
                }
                if (c == '*')
                {
                    params.discard = true;
                    c = *format++;
                }
                if (c=='h')
                {
                    params.param_width = c; /* Half word */
                    c = *format++;
                    if (c=='h')
                    {
                        /* byte */
                        params.param_width = 'b';
                        c = *format++;
                    }
                }
                else if (c=='l')
                {
                    params.param_width = c;
                    c = *format++;
                    if (c=='l')
                    {
                        /* Long-long */
                        params.param_width = 'L';
                        c = *format++;
                    }
                }
                else if (c=='z' || c=='j' || c=='t')
                {
                    params.param_width = 'L'; /* size_t/intmax_t/ptrdiff_t is 64bit */
                    c = *format++;
                }

                switch (c)
                {
                    case 'c':
                        /* Characters */
                        char *cdest;
                        if (params.field_width == 0)
                            params.field_width = 1;
                        if (params.discard)
                            cdest = NULL;
                        else
                            cdest = va_arg(args, char *);

                        for (; (params.field_width > 0) && *str ; params.field_width--)
                        {
                            if (cdest)
                                *cdest++ = *str++;
                            else
                                str++;
                        }
                        if (cdest)
                            converted++;
                        break;

                    case 's':
                        /* String conversion - read up to end of string, or whitespace */
                        char *sdest;
                        if (params.discard)
                            sdest = NULL;
                        else
                            sdest = va_arg(args, char *);
                        //char *odest = sdest;

                        /* Skip any leading spaces */
                        while (*str && isspace(*str))
                            str++;

                        while (*str && !isspace(*str))
                        {
                            if (sdest)
                                *sdest++ = *str++;
                            else
                                str++;
                            if (params.field_width > 0)
                            {
                                params.field_width--;
                                if (params.field_width == 0)
                                    break;
                            }
                        }
                        if (sdest)
                            *sdest = '\0';

                        //printf("Converted '%s', left '%s', format '%s'\n", odest, str, format);
                        if (sdest)
                            converted++;
                        if (*str == '\0')
                        {
                            /* We stopped at terminator */
                            goto done;
                        }
                        break;

                    case 'd':
                    case 'i':
                    case 'u':
                    case 'x':
                    case 'X':
                    case 'o':
                        /* Integer conversion */
                        void *dest;
                        bool negative = false;
                        uint64_t value = 0;
                        int digits = 0;

                        dest = params.discard ? NULL : va_arg(args, void *);

                        /* Skip any leading spaces */
                        while (*str && isspace(*str))
                            str++;

                        if (*str == '-')
                        {
                            negative = true;
                            str++;
                        }
                        if ((c == 'i' || c == 'x' || c == 'X') && *str == '0' && (str[1] | 32) == 'x')
                        {
                            str += 2;
                            goto is_hex;
                        }
                        else if (c == 'x' || c == 'X')
                        {
                    is_hex:
                            /* Hex sequence (only valid for 'i' or 'x') */
                            while ((*str >= '0' && *str <= '9') ||
                                   ((*str | 32) >= 'a' && (*str | 32) <= 'f'))
                            {
                                c = *str++;
                                if (c >= '0' && c <= '9')
                                    value = value * 16 + (c - '0');
                                else
                                    value = value * 16 + (10 + (c|32) - 'a');
                                digits++;
                            }
                        }
                        else if ((c == 'i' || c =='o') && *str == '0' && str[1] >= '0' && str[1] <= '7')
                        {
                            str += 1;
                            goto is_octal;
                        }
                        else if (c =='o')
                        {
                    is_octal:
                            /* Octal sequence (only valid for 'i') */
                            while (*str >= '0' && *str <= '7')
                            {
                                c = *str++;
                                if (c >= '0' && c <= '7')
                                    value = value * 8 + (c - '0');
                                digits++;
                            }
                        }
                        else
                        {
                            /* Base 10 */
                            //printf("sscanf(base10): '%s'\n", str);
                            while (*str >= '0' && *str <= '9')
                            {
                                value = value * 10 + (*str++ - '0');
                                digits++;
                            }
                        }
                        if (digits == 0)
                            goto done; /* Wasn't a number */
                        if (negative)
                            value = -value;
                        //printf("value = %i\n", value);

                        if (dest)
                        {
                            if (params.param_width == 'b')
                                *(char *)dest = value;
                            else if (params.param_width == 'h')
                                *(unsigned short *)dest = value;
                            else if (!params.param_width)
                                *(uint32_t *)dest = value;
                            else
                                *(uint64_t *)dest = value;

                            converted++;
                        }
                        break;

                    case 'n':
                        int *ndest;
                        ndest = params.discard ? NULL : va_arg(args, void *);
                        if (ndest)
                            *ndest = str - start;
                        break;

                }
                break;

            default:
                /* Anything else must match exactly */
                if (*str++ != c)
                    goto done;
                break;
        }
    }

done:
    return converted;
}
