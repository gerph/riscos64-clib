#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "clib.h"
#include "kernel/kernel-state.h"
#include "heap/init.h"
#include "swis_os.h"
#include "env.h"
#include "constructors.h"

/* Define this to put the arguments on the stack, rather then in the heap */
#define BUILD_ARGV_ON_STACK

/* Define this to support the redirection on the command line */
#define CLI_REDIRECTION

/* Include debugging for the redirection operations */
//#define DEBUG_REDIRECTION

/* The minimum heap we want to have */
#define MINIMUM_HEAP (4*1024)

/* The minimum heap we want to have */
#define DEFAULT_STACK (64*1024)



extern int main(int argc, const char *argv[]);

void *__aif_base;
void *__aif_end;
void *__memory_end;
uint32_t __attribute__((weak)) __aif_stacksize;
uint32_t __attribute__((weak)) __heap_min;

static void __main_fail(const char *msg)
{
    os_write0(msg);
    os_newline();
    exit(1);
}


#ifdef CLI_REDIRECTION
#include <stdio.h>
/*************************************************** Gerph *********
 Function:      __cli_redirection
 Description:   Manipulate the arguments to redirect input and output
 Parameters:    argc = number of arguments
                argv-> the arguments decoded
 Returns:       none
 ******************************************************************/
void __cli_redirection(int argc, char **argv)
{
    int i;
    for (i=0; i < argc; i++)
    {
        int oldi = i;
        char *arg = argv[i];
        int fd = -1;
        char *filename;
        bool append = false;
        if (arg[0] == '<')
        {
            /* This *MIGHT* be an environment variable */
            int o;
            for (o=1; arg[o] != '\0'; o++)
            {
                if (arg[o] == '<' || arg[o] == ' ')
                {
                    /* This is something like "<<" or "< <", so it's not really a redirection -
                     * redirecting to sysvars should use the next arg.
                     */
                    goto not_redirection;
                }
                if (arg[o] == '>')
                {
                    /* This is something like "<65>" or "<var$name>", so not really a redirection */
                    goto not_redirection;
                }
            }
            fd = 0;
            arg++;
        }
        else
        {
            if ((arg[0] == '2' || arg[0] == '1') && arg[1] == '>')
            {
                fd = (*arg++) - '0';
                arg++;
            }
            else if (arg[0] == '>')
            {
                fd = 1;
                arg++;
            }
            else
            {
                goto not_redirection;
            }
            if (*arg == '&')
            {
                /* FIXME: This is a dup'd handle; not supported yet */
                __main_fail("Redirection to other handles not supported");
            }

            if (*arg == '>')
            {
                append = true;
                arg++;
            }
        }

        if (*arg == ' ')
        {
            /* Definitely not a redirection if it's in a single arg */
            goto not_redirection;
        }
        if (*arg == '\0')
        {
            /* Format: "> filename" */
            i++;
            if (i == argc)
                goto bad_redirection;
            filename = argv[i];
        }
        else
        {
            filename = arg;
        }

        if (fd == 0)
        {
            FILE *fh;
#ifdef DEBUG_REDIRECTION
            os_write0("Redirect input: filename = "); os_write0(filename); os_newline();
#endif
            fh = freopen(filename, "r", stdin);
            if (fh == NULL)
            {
                __main_fail("Input redirection failed");
            }
            stdin = fh;
        }
        else if (fd == 1 || fd == 2)
        {
            FILE **fhp = (fd == 1) ? &stdout : &stderr;
            FILE *fh;
#ifdef DEBUG_REDIRECTION
            os_write0("Redirect output: filename = "); os_write0(filename); os_newline();
#endif
            fh = freopen(filename, append ? "a" : "w", *fhp);
            if (fh == NULL)
            {
                __main_fail("Output redirection failed");
            }
            *fhp = fh;
        }

        // Now we need to remove the arguments from the argv array.
        int o;
        int nexti = oldi - 1;
        argc -= (i - oldi) + 1;
        for (o = i + 1; oldi < argc; o++, oldi++)
        {
            argv[oldi] = argv[o];
        }
        i = nexti;
not_redirection:
    }
#ifdef DEBUG_REDIRECTION
    os_write0("Post-redirection args:"); os_newline();
    for (i=0; i<argc; i++)
        os_write0("  "), os_write0(argv[i]), os_newline();
#endif
    return;

bad_redirection:
    __main_fail("Bad redirection");
}
#endif

int __main(const char *cli,
           const void *appspace,
           void *append,
           void *memend)
{
    int rc;

    /* We want to initialise the environment so that we exit cleanly */
    _env_init();

    /**** Check out memory is sufficient and that we can allocate heap ****/
    /* WARNING: The stack limit is not currently enforced here */

    if (memend < append)
    {
        __main_fail("Not enough memory for application");
    }

    /* Malloc heap starts at append, and runs to the end of memory, less stack */
    uint64_t stack_size = __aif_stacksize ? __aif_stacksize : DEFAULT_STACK;
    uint64_t stack_limit = ((uint64_t)memend) - stack_size;
    if (stack_limit < (uint64_t)append)
    {
        __main_fail("Not enough memory for stack");
    }
    uint64_t heap_min = __heap_min ? __heap_min : MINIMUM_HEAP;
    if (stack_limit < (uint64_t)append + heap_min)
    {
        __main_fail("Not enough memory for heap");
    }
    _kernel_inmodule = false;
    __heap_init(append, (void*)stack_limit);

    /* Initialise all our libraries now that we safely have a heap */
    _clib_internalinit();

    /**** Build argv ****/

#ifdef BUILD_ARGV_ON_STACK
    int spacecli = strlen(cli) + 1;
    char arg[spacecli];
#else
    char *arg = malloc(spacecli);
    if (!arg)
    {
        __main_fail("Not enough memory for CLI");
    }
#endif
    memcpy(arg, cli, spacecli);

    char *p;
    int argc = 0;
    int in_spaces = 1;
    int in_quotes = 0;
    int escape = 0;
    for (p=arg; *p; p++)
    {
        if (*p == '\\')
        {
            escape = 1;
            continue;
        }
        if (escape)
        {
            escape = 0;
            continue;
        }

        if (!in_quotes)
        {
            if (in_spaces)
            {
                if (*p != ' ')
                {
                    /* No longer in spaces, so this is an argument */
                    argc++;
                    in_spaces = 0;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (*p == ' ')
                {
                    in_spaces = 1;
                    continue;
                }
            }
            if (*p == '"')
                in_quotes = 1;
        }
        else
        {
            if (*p == '"')
            {
                in_quotes = 0;
            }
        }
    }

    if (in_quotes)
        __main_fail("Mismatched quotes in command line arguments");

    /* We now know how many arguments we have, so we can initialise argv */
#ifdef BUILD_ARGV_ON_STACK
    char *argv[argc + 1];
#else
    char **argv = calloc(sizeof(const char *), (argc + 1));
    if (!argv)
    {
        __main_fail("Not enough memory for CLI array");
    }
#endif
    in_spaces = 1;
    in_quotes = 0;
    escape = 0;
    argc = 0;
    char *argout=arg;
    for (p=arg; *p; p++)
    {
        if (*p == '\\')
        {
            escape = 1;
            continue;
        }
        if (escape)
        {
            escape = 0;
            if (*p != '"')
                *argout++ = '\\';
            *argout++=*p;
            continue;
        }

        if (!in_quotes)
        {
            if (in_spaces)
            {
                if (*p != ' ')
                {
                    /* No longer in spaces, so this is an argument */
                    argv[argc] = argout;
                    argc++;
                    in_spaces = 0;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (*p == ' ')
                {
                    *argout++ = '\0';
                    in_spaces = 1;
                    continue;
                }
            }

            if (*p == '"')
                in_quotes = 1;
            else
                *argout++=*p;
        }
        else
        {
            if (*p == '"')
            {
                in_quotes = 0;
            }
            else
            {
                *argout++=*p;
            }
        }
    }

    argv[argc] = NULL;

#ifdef CLI_REDIRECTION
    __cli_redirection(argc, argv);
#endif

#ifdef CONSTRUCTORS
    __constructors_init();
#endif

    /*** Call main ***/
    rc = main(argc, (const char **)argv);

    exit(rc);
    return rc;
}
