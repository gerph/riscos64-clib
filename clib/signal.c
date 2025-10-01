/*******************************************************************
 * File:        signal
 * Purpose:     Trapped signal handling
 * Author:      Gerph
 * Date:        2025-04-10
 ******************************************************************/

#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "kernel.h"
#include "kernel/kernel-errors.h"
#include "swis.h"
#include "swis_os.h"
#include "conversion/cvt.h"


#define SIGMAX (7)
static uint64_t signals_blocked; /* WARNING: Assumes < 64 signals */

#define signal_ignore (NULL)

static const struct {
    int32_t errnum;
    char errmess[8];
} err_Escape = { ErrorNumber_Escape, "Escape" };

/*************************************************** Gerph *********
 Function:      signal_escape
 Description:   Manage the handling of the escape key
 Parameters:    sig = signal number (usually SIGINT)
 Returns:       none
 ******************************************************************/
static void signal_escape(int sig)
{
    /* For simplicity, just raise the escape error */
    os_generateerror(&err_Escape);
}


/*************************************************** Gerph *********
 Function:      signal_escape
 Description:   Manage the handling of the escape key
 Parameters:    sig = signal number (usually SIGINT)
 Returns:       none
 ******************************************************************/
static void signal_abort(int sig)
{
    os_write0("Aborted");
    os_newline();
    os_newline();

    _kernel_backtrace();
    exit(1);
}


static sig_t handlers[SIGMAX];
static sig_t default_handlers[SIGMAX] = {
    [SIGHUP] = signal_ignore,
    [SIGINT] = signal_escape,
    [SIGILL] = signal_ignore,
    [SIGABRT] = signal_abort,
};


/*************************************************** Gerph *********
 Function:      __signal_init
 Description:   Initialise the signal system to its default state
 Parameters:    none
 Returns:       none
 ******************************************************************/
void __signal_init(void)
{
    memcpy(handlers, default_handlers, sizeof(handlers));
    signals_blocked = 0;
}


/*************************************************** Gerph *********
 Function:      signal
 Description:   Change the trapping for a given signal
 Parameters:    sig = the signal we're trapping
                func = the signal handler (or SIG_IGN, SIG_DFL)
 Returns:       the old signal handler, or SIG_ERR if failed
 ******************************************************************/
sig_t signal(int sig, sig_t func)
{
    sig_t prior_signal;

    if (sig >= SIGMAX)
        return SIG_ERR;

    prior_signal = handlers[sig];
    if (prior_signal == default_handlers[sig])
        prior_signal = SIG_DFL;
    else if (prior_signal == signal_ignore)
        prior_signal = SIG_IGN;

    handlers[sig] = func;

    return prior_signal;
}



/*************************************************** Gerph *********
 Function:      raise
 Description:   Raise a signal
 Parameters:    sig = the signal we're raising
 Returns:       0 if successful, -1 if failed
 ******************************************************************/
int raise(int sig)
{
    bool is_blocked;
    sig_t func;
    if (sig >= SIGMAX)
        return -1;

    is_blocked = (signals_blocked & (1lu<<sig)) != 0;
    if (is_blocked)
        func = default_handlers[sig];
    else
        func = handlers[sig];
    signals_blocked |= (1lu<<sig);

    if (func != NULL)
        func(sig);

    signals_blocked &= ~(1lu<<sig);

    return 0;
}

