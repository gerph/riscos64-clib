/*******************************************************************
 * File:        clib
 * Purpose:     C library maintenance
 * Author:      Gerph
 * Date:        25 May 2025
 ******************************************************************/

#include <stdlib.h>
#include "kernel.h"
#include "kernel/kernel-state.h"
#include "time/time-clock.h"
#include "signal-init.h"
#include "heap/init.h"
#include "io/io-init.h"
#include "sysvars/sysvars-init.h"
#include "constructors.h"
//#include "swis_os.h"
#include "clib.h"

/*************************************************** Gerph *********
 Function:      _clib_internalinit
 Description:   Internal initialisation of the C library modules
                We just set up the library initialisations that we
                need, so that each library is in a consistent state.
                Although most variables could be empty, we might
                have some left over data from having been saved out
                of memory. For that reason, and to allow us to be
                restarted cleanly if we do that in the future, we
                try to ensure that each module's initialisation
                is called.
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _clib_internalinit(void)
{
    _kernel_init();
    __signal_init();
    __clock_init();
    __getenv_init();
    __io_init();
}


/*************************************************** Gerph *********
 Function:      _clib_finalise
 Description:   Shut down all our C libraries, triggering atexit if needed.
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _clib_finalise(void)
{
#ifdef CONSTRUCTORS
    __constructors_final();
#endif
    __atexit_trigger();

    /* Shut down our modules */
    __getenv_final();
    __io_final();
    // __heap_final();
}


void __attribute__((noreturn)) exit(int rc)
{
    _clib_finalise();

    _Exit(rc);
}
