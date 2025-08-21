#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char commands[1024] = "";
    int i;
    int rc;
    if (argc == 1)
    {
        printf("Syntax: %s <command>*\n", argv[0]);
        exit(1);
    }

    for (i=1; i<argc; i++)
    {
        if (i>1)
            strcat(commands, " ");
        strcat(commands, argv[i]);
        printf("%i: %s : %s\n", i, argv[i], commands);
    }

    printf("Running: %s\n", commands);
    rc = system(commands);
    printf("Returned, with rc=%i\n", rc);

    return 0;
}
