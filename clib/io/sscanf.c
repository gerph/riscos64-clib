/* Very very simple sscanf */

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
    char c;
    //printf("vsscanf '%s' with '%s'\n", str, format);
    while (*format)
    {
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

                switch (c)
                {
                    case 's':
                        /* String conversion - read up to end of string, or whitespace */
                        char *sdest = va_arg(args, char *);
                        //char *odest = dest;

                        /* Skip any leading spaces */
                        while (*str && isspace(*str))
                            str++;

                        while (*str && !isspace(*str))
                            *sdest++ = *str++;
                        *sdest = '\0';
                        //printf("Converted '%s', left '%s', format '%s'\n", odest, str, format);
                        converted++;
                        if (*str == '\0')
                        {
                            /* We stopped at terminator */
                            goto done;
                        }
                        break;

                    case 'd':
                    case 'i':
                        /* Integer conversion */
                        int *idest = va_arg(args, int);
                        bool negative = false;
                        int value = 0;
                        int digits = 0;
                        if (*str == '-')
                        {
                            negative = true;
                            str++;
                        }
                        if (c == 'i' && *str == '0' && (str[1] | 32) == 'x')
                        {
                            str += 2;
                            /* Hex sequence (only valid for 'i') */
                            while ((*str >= '0' && *str <= '9') ||
                                   ((*str | 32) >= 'a' && (*str | 32) <= 'f'))
                            {
                                c = *str++;
                                if (c >= '0' && c <= '9')
                                    value = value * 16 + (c - '0');
                                else
                                    value = value * 16 + ((c|32) - 'a');
                                digits++;
                            }
                        }
                        else if (c == 'i' && *str == '0' && str[1] >= '0' && str[1] <= '7')
                        {
                            str += 1;
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
                        *idest = value;
                        converted++;
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
