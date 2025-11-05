#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "kernel.h"

/**
 * Generate a temporary filename with a given signature.
 *
 * @return pointer to name, or NULL if no name could be generated.
 */
static char *_sys_tmpnam_(char *name, int sig)
{
    char *tail;
    bool use_dollar = false;
    /* JRF: I'm not happy with this; it means that the Wimp$ScrapDir path
            is expanded unnecessarily - we could just use the variable name
            in the filename produced. */
    if (_kernel_getenv("Wimp$ScrapDir", name, L_tmpnam-10) != NULL)
    {
      strcpy(name, "$.tmp");
      use_dollar = true;
    }

    {
        /* Check that the directory exists */
        int objtype = _kernel_osfile(5, name, NULL);
        /* The name location must be a directory (or an image) */
        if (objtype != 2 && objtype != 3)
        {
            if (use_dollar)
            {
                /* This is a very minimal system without Wimp$ScrapDir, so fall back to $ */
                strcpy(name, "$");
            }
            else
            {
                return NULL;
            }
        }
    }

    tail = name + strlen(name);
    sprintf(tail, ".x%.8x", sig);
    return name;
}


static char _tmp_file_name[L_tmpnam] = "";
static int counter = 0;

#define getsig() (clock() ^ counter++)

char *tmpnam(char *name)
{
    int attempts = 10;
    if (name == NULL)
        name = _tmp_file_name;

    do {
      int sig = getsig();
      /* JRF: Return NULL if we cannot generate a temporary filename. */
      if (!_sys_tmpnam_(name, sig))
      {
        errno = ENOENT;
        return NULL;
      }
      attempts--;
    } while (_kernel_osfile(17, name, NULL) != 0 || attempts == 0);

    if (attempts == 0)
    {
        /* Failure */
        errno = ENOENT;
        return NULL; /* Many attempts made and we were not successful */
    }

    return name;
}


char *mktemp(char *template)
{
    char *marker = template + strlen(template) - 1;
    int attempts = 10;
    if (marker <= template)
        return NULL;
    if (*marker != 'X')
        return NULL; /* There were no 'X' characters present */
    while (marker > template && *marker == 'X')
        marker--;

    while (1)
    {
        int sig = getsig();
        const char dict[33] = "ABCDEFGHIJKLMNOPQRSTYUVWXYZ" // 26
                              "123456"; // 32
        char *p = marker;
        int i=0;
        while (*p != '\0')
        {
            int c = dict[(sig>>i) & 31];
            i += 5;
            if (i >= 31 - 5)
                i -= 25;
            *p++ = c;
        }
        attempts --;
    } while (_kernel_osfile(17, template, NULL) != 0 || attempts == 0);
    return template;
}
