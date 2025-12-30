
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void test_multiargs(void)
{
    char *parse = "r255g0b0";
    int r,g,b;
    int args;

    printf("--- Multiargs ---\n");

    printf("Parsing '%s'\n", parse);
    args = sscanf(parse,"r%dg%db%d",&r,&g,&b);
    if (args==3) {
        printf("Got 3 values: %i, %i, %i\n", r, g, b);
        if (r == 255 && g == 0 && b==0)
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
            exit(1);
        }
    }
    else
    {
        printf("Got %i args - FAILED\n", args);
        exit(1);
    }
}

#define SSCANF_CHECK(format, str, decoded, expect) \
    printf("Parse '%s':", str); \
    args = sscanf(str, format, &got); \
    if (args==1) { \
        if (decoded) { \
            printf("  Got "format", expected "format": ", got, expect); \
            if (got == expect) { \
                printf("OK\n"); \
            } else { \
                printf("FAILED\n"); \
                exit(1); \
            } \
        } else { \
            printf("  Got result, but didn't expect one: FAILED\n"); \
            exit(1); \
        } \
    } else { \
        if (decoded) { \
            printf("Got %i args: FAILED\n", args); \
            exit(1); \
        } else { \
            printf("Couldn't decode (expected): OK\n"); \
        } \
    }

void test_decimal(void)
{
    int got;
    int args;

    printf("--- Decimal ---\n");

    SSCANF_CHECK("%d", "1234", true, 1234);
    SSCANF_CHECK("%d", "0", true, 0);
    SSCANF_CHECK("%d", "b0b", false, 0);
    SSCANF_CHECK("%d", "-1", true, -1);
    SSCANF_CHECK("%d", "0x1234", true, 0);
    SSCANF_CHECK("%d", "055", true, 55);
}

void test_integer(void)
{
    int got;
    int args;

    printf("--- Integer ---\n");

    SSCANF_CHECK("%i", "1234", true, 1234);
    SSCANF_CHECK("%i", "0", true, 0);
    SSCANF_CHECK("%i", "b0b", false, 0);
    SSCANF_CHECK("%i", "-1", true, -1);

    SSCANF_CHECK("%i", "0x1234", true, 0x1234);
    SSCANF_CHECK("%i", "055", true, 055);
    SSCANF_CHECK("%i", "-0x1234", true, -0x1234);
}


int main(int argc, char *argv[])
{
    test_decimal();
    test_integer();
    test_multiargs();

    return 0;
}
