#include <stdio.h>
#include <stdlib.h>
#include "kernel.h"
#include "swis.h"

int rc = 0;

void assert_eq(int a, int b)
{
    if (a != b)
    {
        printf("Failed assertion (&%x != &%x ; %i != %i)\n", a, b, a, b);
        rc = 1;
    }
}

void assert_err(_kernel_oserror *a, _kernel_oserror *b)
{
    if (a == NULL)
    {
        printf("Failed error check (should have reported error %i:%s)\n", b->errnum, b->errmess);
        rc = 1;
    }
    else
    {
        if (a != b)
        {
            /* Different error block, which isn't strictly the same as Classic but not terrible */
            if (a->errnum != b->errnum || strcmp(a->errmess, b->errmess) != 0)
            {
                printf("Failed error check (should have reported error %i:%s\n", b->errnum, b->errmess);
                printf("                                       but got %i:%s)\n", a->errnum, a->errmess);
                rc = 1;
            }
        }
    }
}

_kernel_oserror err_Example = { 42, "My Error" };

int main(int argc, char **argv)
{
    _kernel_oserror *err;

    assert_eq(_IN(0), 1);
    assert_eq(_IN(1), 2);
    assert_eq(_INR(0,1), 3);
    assert_eq(_INR(1,3), 14);
    assert_eq(_OUT(1), (1u<<30));
    assert_eq(_OUT(0), (1u<<31));
    assert_eq(_OUTR(0,1), (3u<<30));
    assert_eq(_OUTR(1,3), (7u<<28));
    assert_eq(_INR(0,1) | _OUTR(1,3), (7u<<28) | (3));

    _swix(OS_Write0, _IN(0), "Hello world");
    _swix(OS_NewLine, 0);
    _swix(OS_WriteN, (_IN(0)) | (_IN(1)), "OS_WriteN", 9);
    _swix(OS_NewLine, 0);

    err = _swix(OS_GenerateError, _IN(0), &err_Example);
    assert_err(err, &err_Example);

    return rc;
}
