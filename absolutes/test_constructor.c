#include "swis_os.h"

__attribute__((constructor)) void my_construct(void)
{
    os_write0("in my_construct");
    os_newline();
}

__attribute__((destructor)) void my_destruct(void)
{
    os_write0("in my_destruct");
    os_newline();
}

int main(int argc, char *argv[])
{
    os_write0("in main");
    os_newline();
    return 0;
}
