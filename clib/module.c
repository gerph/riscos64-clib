/*******************************************************************
 * File:        module
 * Purpose:     Module initialisation/finalisation functions
 * Author:      Gerph
 * Date:        10 Aug 2024
 ******************************************************************/

#include <stdlib.h>
#include "kernel.h"
#include "clib.h"
#include "kernel/kernel-state.h"
#include "constructors.h"


_kernel_oserror *_clib_initialisemodule(void *pwp)
{
    /* Perform zero-init ? */
    void __attribute__((weak)) __RelocCode(void);
    if (__RelocCode)
        __RelocCode();
    _kernel_inmodule = true;
    _clib_internalinit();

#ifdef CONSTRUCTORS
    __constructors_init();
#endif
    return NULL;
}

_kernel_oserror *_clib_finalisemodule(void *pwp)
{
    extern void _clib_finalise(void);
    _clib_finalise();
    return NULL;
}
