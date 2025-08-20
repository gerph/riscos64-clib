#include "swis_os.h"

static int counter = 0;

/* Will be called first */
__attribute__((constructor)) void my_construct(void)
{
    os_write0("in my_construct");
    os_newline();
    if (counter++ != 0)
    {
        os_write0("[[counter invalid]]");
        os_newline();
        os_generateerror((_kernel_oserror*)"0000Failed");
    }
}

/* Will be called third (last) */
__attribute__((destructor)) void my_destruct(void)
{
    os_write0("in my_destruct");
    os_newline();
    if (counter++ != 2)
    {
        os_write0("[[counter invalid]]");
        os_newline();
        os_generateerror((_kernel_oserror*)"0000Failed");
    }
}

/* Will be called second */
int main(int argc, char *argv[])
{
    os_write0("in main");
    os_newline();
    if (counter++ != 1)
    {
        os_write0("[[counter invalid]]");
        os_newline();
        os_generateerror((_kernel_oserror*)"0000Failed");
    }
    return 0;
}
