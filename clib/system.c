/*******************************************************************
 * File:        system
 * Purpose:     System commands management
 * Author:      Gerph
 * Date:        8 Aug 2025
 ******************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "kernel.h"
#include "kernel/kernel-state.h"
#include "swis_os.h"
#include "clib.h"
#include "env.h"

//#define NO_COMMAND_PROCESSOR

/*************************************************** Gerph *********
 Function:      system
 Description:   Call a system command
 Parameters:    string-> the string to call
                         May be prefixed by `call:` or `chain:` (insensitive)
 Returns:       return code from the command.
 ******************************************************************/
int system(const char *string)
{
#ifdef NO_COMMAND_PROCESSOR
    /* Without any command processor, we ALWAYS return 0. */
    return 0;
#else
    if (string == NULL)
    {
        /* They asked whether a command processor was available.
         * We only have a command processor if we're not in a module,
         * as it is not safe to call *-commands from SVC mode.
         */
        if (_kernel_inmodule)
            return 0;
        return 1;
    }

    /* We default to using call - starting the process and returning here */
    /* (the alternative is chain, which exits this environment and runs the command) */
    bool uses_call = true;

#define CHECK_CHAR(_index, _char) \
        (((_char >= 'a' && _char <= 'z') ? (string[_index] | 32) : string[_index]) == _char)

    if (CHECK_CHAR(0, 'c'))
    {
        if (CHECK_CHAR(1, 'h'))
        {
            if (CHECK_CHAR(2, 'a') &&
                CHECK_CHAR(3, 'i') &&
                CHECK_CHAR(4, 'n') &&
                CHECK_CHAR(5, ':'))
            {
                string += 6;
                uses_call = false;
            }
        }
        else if (CHECK_CHAR(1, 'a') &&
                 CHECK_CHAR(2, 'l') &&
                 CHECK_CHAR(3, 'l') &&
                 CHECK_CHAR(4, ':'))
        {
            string += 5;
            uses_call = true;
        }
    }

    if (!uses_call)
    {
        /* We need to shut down, then run the command */
        _clib_finalise();
        _env_restore();
        int ok = _kernel_oscli(string);
        if (ok)
            _Exit(0);

        os_generateerror(_kernel_last_oserror());
        /* If that returns, we exit anyhow */
        _Exit(1);
    }

    /* Call'd system commands (ie the ones that are expected to be subprocesses) */
    printf("System commands not currently supported\n");
    abort();
#endif
}
