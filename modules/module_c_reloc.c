/*******************************************************************
 * File:        module
 * Purpose:     Test module for struct data local in functions.
 * Author:      Gerph
 ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kernel.h"
//#include "modhead.h"

int failed = 0;

typedef struct pair_s {
    const char *data;
    int value;
} pair_t;

static const char mystring[] = "HELLO";

const char *GetData(int i)
{
#ifdef TEST_STATIC
    static
#endif
#ifdef TEST_CONST
    const
#endif
    pair_t pairs[5] = {
        {"I", 1},
        {"II", 2},
        {"III", 3},
        {"IV", 2},
        {mystring, 5},
    };
    /* These addresses should be on the stack for a 'const pair_t',
     * and in the module workspace for a 'static const pair_t'.
     * The data pointed to will always be in the module workspace.
     */
    printf("[[%p %p:%i]]", &pairs[i].data, &pairs[i].value, pairs[i].value);
    if (strlen(pairs[i].data) != pairs[i].value)
    {
        printf("[[FAILED - length should be %i]]", pairs[i].value);
        failed = 1;
    }
    return pairs[i].data;
}

_kernel_oserror err_Failed = { 0, "FAILED test" };

/***************************************************************************
 * Function:     Mod_Init
 * Description:  Initialise the module, setting up vectors, callbacks and
 *               any other parts of the system necessary for the module to
 *               function.
 * Parameters:   tail        = pointer to command line (control terminated)
 *               podule_base = address of podule module was started from, or
 *                             NULL if none
 *               pw          = private word for module
 * On exit:      Return NULL for successful initialisation, or a pointer to
 *               an error block if the module could not start properly.
 **************************************************************************/
_kernel_oserror *Mod_Init(const char *tail, int podule_base, void *pw)
{
    printf("Module MyModule initialised\n");
    int i;
    for (i=0; i<5; i++)
    {
        const char *s = GetData(i);
        printf(" %i => %p = %s\n", i, s, s);
    }
    if (failed)
        return &err_Failed;
    return NULL;
}


/***************************************************************************
 * Function:     Mod_Final
 * Description:  Finalise the module, shutting down any systems necessary,
 *               freeing vectors and releasing workspace
 * Parameters:   fatal       = fatality indicator; 1 if fatal, 0 if
 *                             reinitialising
 *               podule_base = address of podule module was started from, or
 *                             NULL if none
 *               pw          = private word for module
 * On exit:      Return 0 for successful finalisation, or a pointer to an
 *               error block if module was not shutdown properly.
 **************************************************************************/
_kernel_oserror *Mod_Final(int fatal, int podule_base, void *pw)
{
    printf("Module MyModule dying\n");
    return NULL;
}

/***************************************************************************
 * Description:  Star command and help request handler routines.
 * Parameters:   arg_string = pointer to argument string (control
 *                            terminated), or output buffer
 *               argc       = number of arguments passed
 *               number     = command number (see CMD_* definitions below)
 *               pw         = private word for module
 * On exit:      If number indicates a help entry:
 *                 To output, assemble zero terminated output into
 *                 arg_string, and return help_PRINT_BUFFER to print it.
 *                 To stay silent, return NULL.
 *                 To given an error, return an error pointer.
 *                 [In this case, you need to cast the 'const' away]
 *               If number indicates a configure option:
 *                 If arg_string is arg_STATUS, then print status, otherwise
 *                 use argc and arg_string to set option.
 *                 Return NULL for no error.
 *                 Return one of the four error codes below (configure_*)
 *                 for a generic error message.
 *                 Return an error pointer for a custom error.
 *               If number indicates a command entry:
 *                 Execute the command given by number, and arg_string.
 *                 Return NULL on success,
 *                 Return a pointer to an error block on failure.
 **************************************************************************/
_kernel_oserror *Mod_Command(const char *arg_string, int argc, int number,
                             void *pw)
{
    /* Nothing here */
  return NULL;
}

/***************************************************************************
 * Description:  SWI handler routine. All SWIs for this module will be
 *               passed to these routines.
 * Parameters:   number = SWI number within SWI chunk (i.e. 0 to 63)
 *               r      = pointer to register block on entry
 *               pw     = private word for module
 * On exit:      Return NULL if SWI handled sucessfully, setting return
 *               register values (r0-r9) in r.
 *               Return error_BAD_SWI for out of range SWIs.
 *               Return an error block for a custom error.
 **************************************************************************/
_kernel_oserror *SWI_Call(int number, _kernel_swi_regs *r, void *pw)
{
    /* Nothing here */
    return NULL;
}

