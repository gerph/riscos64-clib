/*******************************************************************
 * File:        kernel-raiseerror
 * Purpose:     Handling of shutting down the application
 * Author:      Gerph
 * Date:        28 Aug 2025
 ******************************************************************/

#include <stdlib.h>
#include <stdbool.h>

#include "kernel.h"

#include "swis_os.h"
#include "clib.h"
#include "env.h"

/*************************************************** Gerph *********
 Function:      _kernel_raise_error_int
 Description:   Shut down the application and exit with an error
 Parameters:    err-> the error (in our buffer)
 Returns:       never
 ******************************************************************/
void __attribute__((noreturn)) _kernel_raise_error_int(_kernel_oserror *err)
{
    static bool reentered = false;
    if (!reentered)
    {
        reentered = true;
        _clib_finalise();
    }
    _env_restore();
    if (err)
        os_generateerror(err);
    else
        _Exit(1);
}
