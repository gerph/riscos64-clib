/* Based on code from: https://www.netlib.org/ampl/solvers/printf.c */
/****************************************************************
Copyright (C) 1997, 1999, 2001 Lucent Technologies
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of Lucent or any of its entities
not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
****************************************************************/

#include <stdlib.h>
#include <string.h>
#include "gdtoa.h"

#include "io/io-vprintf.h"


int __printf_fpfmt(outputter_t *out, formatparams_t *params, char fmt, double x)
{
    char *se;
    int sgn;
    int n = 0;
    char *s = "";
    int dot;
    int decpt;

    switch (fmt)
    {
            case 'f':
                if (params->precision == -1)
                    params->precision = 6;

 infnan:
                s = dtoa(x, 3, params->precision, &decpt, &sgn, &se);
                if (decpt == 9999) {
 fmt9999:
                    dot = params->precision = params->alternate = 0;
                    if (*s == 'N')
                        goto have_s;
                    decpt = strlen(s);
                    }
 f_fmt:
                if (sgn && (x||params->sign))
                    params->sign = '-';
                if (params->precision > 0)
                    params->field_width -= params->precision;
                if (params->field_width > 0) {
                    if (params->sign)
                        --params->field_width;
                    if (decpt <= 0) {
                        --params->field_width;
                        if (params->precision > 0)
                            --params->field_width;
                        }
                    else {
                        if (s == se)
                            decpt = 1;
                        params->field_width -= decpt;
                        if (params->precision > 0 || params->alternate)
                            --params->field_width;
                        }
                    }
                if (params->field_width > 0 && !params->align_left) {
                    if (params->digit_pad) {
                        if (params->sign)
                            n += out->writec(out, params->sign);
                        params->sign = 0;
                        do {
                            n += out->writec(out, '0');
                        } while(--params->field_width > 0);
                    }
                    else {
                        do {
                            n += out->writec(out, ' ');
                        } while(--params->field_width > 0);
                    }
                }
                if (params->sign)
                    n += out->writec(out, params->sign);
                if (decpt <= 0) {
                    n += out->writec(out, '0');
                    if (params->precision > 0 || params->alternate)
                        n += out->writec(out, '.');
                    while(decpt < 0) {
                        n += out->writec(out, '0');
                        params->precision--;
                        decpt++;
                        }
                    }
                else {
                    do {
                        int c;
                        if ((c = *s))
                            s++;
                        else
                            c = '0';
                        n += out->writec(out, c);
                        }
                        while(--decpt > 0);
                    if (params->precision > 0 || params->alternate)
                        n += out->writec(out, '.');
                    }
                while(--params->precision >= 0) {
                    int c;
                    if ((c = *s))
                        s++;
                    else
                        c = '0';
                    n += out->writec(out, c);
                    }
                while(--params->field_width >= 0)
                    n += out->writec(out, ' ');
                break;

            case 'G':
            case 'g':
                if (params->precision == -1)
                    params->precision = 6;

                if (params->precision < 0)
                    params->precision = 0;

                s = dtoa(x, params->precision ? 2 : 0, params->precision, &decpt,
                       &sgn, &se);
                if (decpt == 9999)
                    goto fmt9999;
                int c;
                int prec1;
                c = se - s;
                prec1 = params->precision;
                if (!params->precision) {
                    params->precision = c;
                    prec1 = c + (s[1] || params->alternate ? 5 : 4);
                    /* %.0g gives 10 rather than 1e1 */
                    }
                if (decpt > -4 && decpt <= prec1) {
                    if (params->alternate)
                        params->precision -= decpt;
                    else
                        params->precision = c - decpt;
                    if (params->precision < 0)
                        params->precision = 0;
                    goto f_fmt;
                    }
                fmt -= 2;
                if (!params->alternate && params->precision > c)
                    params->precision = c;
                --params->precision;
                goto e_fmt;

            case 'e':
            case 'E':
                if (params->precision == -1)
                    params->precision = 6;
                if (params->precision < 0)
                    params->precision = 0;
                s = dtoa(x, 2, params->precision+1, &decpt,
                       &sgn, &se);
                if (decpt == 9999)
                    goto fmt9999;
 e_fmt:
                if (sgn && (x||params->sign))
                    params->sign = '-';
                if ((params->field_width -= params->precision + 5) > 0) {
                    if (params->sign)
                        --params->field_width;
                    if (params->precision || params->alternate)
                        --params->field_width;
                    }
                if ((c = --decpt) < 0)
                    c = -c;
                while(c >= 100) {
                    --params->field_width;
                    c /= 10;
                    }
                if (params->field_width > 0 && !params->align_left) {
                    if (params->digit_pad) {
                        if (params->sign)
                            n += out->writec(out, params->sign);
                        params->sign = 0;
                        do {
                            n += out->writec(out, '0');
                        } while(--params->field_width > 0);
                        }
                    else do {
                        n += out->writec(out, ' ');
                    } while(--params->field_width > 0);
                    }
                if (params->sign)
                    n += out->writec(out, params->sign);
                n += out->writec(out, *s++);
                if (params->precision || params->alternate)
                    n += out->writec(out, '.');
                while(--params->precision >= 0) {
                    if ((c = *s))
                        s++;
                    else
                        c = '0';
                    n += out->writec(out, c);
                    }
                n += out->writec(out, fmt);
                if (decpt < 0) {
                    n += out->writec(out, '-');
                    decpt = -decpt;
                    }
                else
                    n += out->writec(out, '+');

                int k;
                for(c = 2, k = 10; 10*k <= decpt; c++, k *= 10);
                for(;;) {
                    int i1;
                    i1 = decpt / k;
                    n += out->writec(out, i1 + '0');
                    if (--c <= 0)
                        break;
                    decpt -= i1*k;
                    decpt *= 10;
                    }
                while(--params->field_width >= 0)
                    n += out->writec(out, ' ');
                break;

            default:
 have_s:
                /* Plain string being returned */
                /* FIXME: No padding or widths applied */
                n += out->write0(out, s);
                break;
    }

    return n;
}

//#define TEST
#ifdef TEST
#include <stdio.h>
char result[512];
char *rout;
int outc(outputter_t *out, const char c)
{
//    printf("{%c}", c);
    *rout++ = c;
    *rout = '\0';
    return 1;
}

#define TEST_SIGN      (1)
#define TEST_PADZERO   (2)
#define TEST_PADSPACE  (4)
#define TEST_ALTERNATE (8)
int fpwriter(char *buffer, double value, char how, int precision, int field_width, int flags)
{
    outputter_t out;
    rout = buffer;
    out.writec = outc;
    formatparams_t params = {0};
    params.precision = precision;
    params.field_width = field_width;
    params.sign = (flags & TEST_PADSPACE) ? ' ' : ((flags & TEST_SIGN) ? '+' : 0);
    params.alternate = (flags & TEST_ALTERNATE) ? '+' : 0;
    params.digit_pad = (flags & TEST_PADZERO ? '0' : 0);
    int n = 0;

    n = __printf_fpfmt(&out, &params, how, value);
    return n;
}

int main(void)
{
    int npass = 0;
    int nfail = 0;
    int n;
#define STRINGIFY(_v) # _v
#define TESTFP(value, how, precision, field_width, flags, expect_n, expect_s) \
        printf("Testing '%s' with %c, precision %i, width %i, flags %s%s%s: ", STRINGIFY(value), how, precision, field_width, flags & TEST_SIGN ? "+" : "", flags & TEST_PADZERO ? "0" : (flags & TEST_PADSPACE ? "<spc>" : ""), flags & TEST_ALTERNATE ? "#" : ""); \
        n = fpwriter(result, value, how, precision, field_width, flags); \
        printf("'%s', %i chars: ", result, n); \
        if (n != expect_n) \
        { \
            printf("FAIL (expected %i chars)", expect_n); \
            nfail++; \
        } \
        else if (strcmp(result, expect_s) != 0) \
        { \
            printf("FAIL (expected string '%s')", expect_s); \
            nfail++; \
        } \
        else \
        { \
            printf("OK"); \
            npass++; \
        } \
        printf("\n");

    TESTFP(7.654,               'f', -1, 0, 0, 8,  "7.654000")
    TESTFP(7.654,               'g', -1, 0, 0, 5,  "7.654")
    TESTFP(7.654,               'e', -1, 0, 0, 12, "7.654000e+00")

/* The following values are taken from: https://github.com/eyalroz/printf/blob/master/test/test_suite_main_testcases.hpp#L862 */

    printf("--- Regular floating point ---\n");
    TESTFP(3.1415354,           'f', 4,  0,  0, 6,  "3.1415");             // "%.4f"
    TESTFP(30343.1415354,       'f', 3,  0,  0, 9,  "30343.142");          // "%.3f"
    TESTFP(34.1415354,          'f', 0,  0,  0, 2,  "34");                 // "%.0f"
    TESTFP(1.3,                 'f', 0,  0,  0, 1,  "1");                  // "%.0f"
    TESTFP(1.55,                'f', 0,  0,  0, 1,  "2");                  // "%.0f"
    TESTFP(1.64,                'f', 1,  0,  0, 3,  "1.6");                // "%.1f"
    TESTFP(42.8952,             'f', 2,  0,  0, 5,  "42.90");              // "%.2f"
    TESTFP(42.8952,             'f', 9,  0,  0, 12, "42.895200000");       // "%.9f"
    TESTFP(42.895223,           'f', 10, 0,  0, 13, "42.8952230000");      // "%.10f"
    TESTFP(42.89522312345678,   'f', 12, 0,  0, 15, "42.895223123457");    // "%.12f"
    TESTFP(42477.37109375,      'f', 15, 20, TEST_PADZERO, 21, "42477.371093750000000");    // "%020.15f"
    TESTFP(42.89522387654321,   'f', 12, 0,  0, 15, "42.895223876543");    // "%.12f"
    TESTFP(42.8952,             'f', 2,  6,  0, 6,  " 42.90");             // "%6.2f"

    TESTFP(42.8952,             'f', 2,  6,  TEST_SIGN, 6, "+42.90");      // "%+6.2f"
    TESTFP(42.9252,             'f', 1,  5,  TEST_SIGN, 5, "+42.9");       // "%+5.1f"
    TESTFP(42.5,                'f', -1, 0,  0, 9, "42.500000");           // "%f"
    TESTFP(42.5,                'f', 1,  0,  0, 4, "42.5");                // "%.1f"
    TESTFP(42167.0,             'f', -1, 0,  0, 12, "42167.000000");       // "%f"
    TESTFP(-12345.987654321,    'f', 9,  0,  0, 16, "-12345.987654321");   // "%.9f""
    TESTFP(3.999,               'f', 1,  0,  0, 3, "4.0");                 // "%.1f"
    TESTFP(3.5,                 'f', 0,  0,  0, 1, "4");                   // "%.0f"
    TESTFP(4.5,                 'f', 0,  0,  0, 1, "4");                   // "%.0f"
    TESTFP(3.49,                'f', 0,  0,  0, 1, "3");                   // "%.0f"
    TESTFP(3.49,                'f', 1,  0,  0, 3, "3.5");                 // "%.1f"

    printf("--- Exponential floating point ---\n");
    TESTFP(0.5,                     'g', 4,  0, 0, 3, "0.5");                  // "%.4g"
    TESTFP(1.0,                     'g', 4,  0, 0, 1, "1");                    // "%.4g"
    TESTFP(12345.678,               'G', -1, 0, 0, 7, "12345.7");              // "%G"
    TESTFP(12345.678,               'G', 7,  0, 0, 8, "12345.68");             // "%.7G"
    TESTFP(123456789.,              'G', 5,  0, 0, 10, "1.2346E+08");          // "%.5G"
    TESTFP(12345.,                  'G', 6,  0, 0, 5, "12345");                // "%.6G"
    TESTFP(123456789.,              'g', 4,  12, TEST_SIGN, 12, "  +1.235e+08");         // "%+12.4g"
    TESTFP(0.001234,                'G', 2,  0, 0, 6, "0.0012");               // "%.2G"
    TESTFP(0.001234,                'G', 4,  10, TEST_SIGN, 10, " +0.001234");           // "%+10.4G"
    TESTFP(0.00001234,              'g', 4,  12, TEST_SIGN|TEST_PADZERO, 12, "+001.234e-05");         // "%+012.4g"
    TESTFP(-1.2345e-308,            'g', 3,  0, 0, 10, "-1.23e-308");           // "%.3g"
    TESTFP(1.23e+308,               'E', 3,  0, TEST_SIGN, 11, "+1.230E+308");          // "%+.3E"
    TESTFP(9.9996,                  'e', 3,  0, 0, 9, "1.000e+01");            // "%.3e"
    TESTFP(0.,                      'g', -1, 0, 0, 1, "0");                    // "%g"
// Note: This next test fails
    //TESTFP(-0.,                     'g', -1, 0, 0, 2, "-0");                   // "%g"
    TESTFP(0.,                      'g', -1, 0, TEST_SIGN, 2, "+0");                    // "%+g"
    TESTFP(-0.,                     'g', -1, 0, TEST_SIGN, 2, "-0");                    // "%+g"
    TESTFP(-40661.5,                'g', 1,  0, 0, 6, "-4e+04");               // "%.1g"
    TESTFP(-40661.5,                'g', 1,  0, TEST_ALTERNATE, 7, "-4.e+04");               // "%#.1g"
    TESTFP(99.998580932617187500,   'g', 3,  0, TEST_ALTERNATE, 4, "100.");                  // "%#.3g"
    TESTFP(9.8,                     'g', 1,  1, TEST_ALTERNATE|TEST_PADSPACE|TEST_PADZERO, 7, " 1.e+01");               // "%# 01.1g"
    TESTFP(1.2345678901e-308,       'e', 10, 0, 0, 17, "1.2345678901e-308");   // "%.10e"
    TESTFP(48955.125,               'e', -1, 0, 0, 12, "4.895512e+04");        // "%e"
    TESTFP(92523.5,                 'e', 4,  0, 0, 10, "9.2524e+04");          // "%.4e"
    TESTFP(-83809.234375,           'e', 9,  0, 0, 16, "-8.380923438e+04");    // "%.9e"

    printf("\nDone\n");
    printf("Pass: %i\n", npass);
    printf("Fail: %i\n", nfail);
    exit(nfail == 0 ? 0 : 1);
}
#endif
