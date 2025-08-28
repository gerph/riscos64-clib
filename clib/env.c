/*******************************************************************
 * File:        env
 * Purpose:     Program environment handlers
 * Author:      Gerph
 * Date:        2025-04-06
 ******************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include "kernel.h"
#include "env.h"
#include "swis_os.h"
#include "clib.h"

#include "envnumbers.h"

typedef intptr_t codeptr_t;

typedef struct handlers_s {
    uint64_t        memory_limit;
    intptr_t        memory_unused_r2;
    intptr_t        memory_unused_r3;

    codeptr_t       abort_undef_handler;
    intptr_t        abort_undef_unused_r2;
    intptr_t        abort_undef_unused_r3;

    codeptr_t       abort_prefetch_handler;
    intptr_t        abort_prefetch_unused_r2;
    intptr_t        abort_prefetch_unused_r3;

    codeptr_t       abort_data_handler;
    intptr_t        abort_data_unused_r2;
    intptr_t        abort_data_unused_r3;

    codeptr_t       abort_address_handler;
    intptr_t        abort_address_unused_r2;
    intptr_t        abort_address_unused_r3;

    codeptr_t       abort_exception_handler;
    intptr_t        abort_exception_unused_r2;
    intptr_t        abort_exception_unused_r3;

    codeptr_t       error_handler;
    intptr_t        error_workspace;
    _kernel_oserror *error_buffer;

    codeptr_t       callback_handler;
    intptr_t        callback_workspace;
    intptr_t        callback_registers;

    /* Never touch 8 Breakpoint */

    codeptr_t       escape_handler;
    intptr_t        escape_workspace;
    intptr_t        escape_unused_r3;

    codeptr_t       event_handler;
    intptr_t        event_workspace;
    intptr_t        event_unused_r3;

    codeptr_t       exit_handler;
    intptr_t        exit_workspace;
    intptr_t        exit_unused_r3;

    /* Never touch 12 Unused SWI */
    /* Never touch 13 Exception registers */
    /* Never touch 14 Application Space  */
    /* Never touch 15 CAO */

    codeptr_t       upcall_handler;
    intptr_t        upcall_workspace;
    intptr_t        upcall_unused_r3;
} handlers_t;

static handlers_t oldhandlers;

riscos_error_buffer_t _kernel_error_buffer;

#define ENV_READ ((intptr_t)0)

static bool env_initialised = false;

/*************************************************** Gerph *********
 Function:      _env_escape
 Description:   Environment handler when escape pressed
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_escape(void)
{
    static int reentered = 0;
    if (reentered)
        return;
    reentered = 1;
    os_byte_out1(0x7e, 0, 0); /* Acknowledge escape */
    reentered = 0;
    raise(SIGINT);
}

/*************************************************** Gerph *********
 Function:      _env_error
 Description:   Environment handler for an error
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_error(_kernel_oserror *err)
{
    static int count = 0;

    count++;

    os_newline();
    if (count == 1 || count == 2)
    {
        if (count == 1)
            os_write0("Error: ");
        else
            os_write0("Re-entered error handler with error: ");
        os_write0(err->errmess);
        os_newline();

        _kernel_backtrace();
    }
    else
    {
        os_write0("Re-entered error handler (possible recursion)");
        os_newline();
    }

    if (count < 3)
        _clib_finalise();
    _env_restore();
    os_generateerror(err);
}

/*************************************************** Gerph *********
 Function:      _env_exit
 Description:   Environment handler when exit is called
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_exit(_kernel_oserror *err)
{
    _clib_finalise();
    _Exit(0);
}


/*************************************************** Gerph *********
 Function:      _env_init
 Description:   Initialise our program environment handlers
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_init(void)
{
    env_initialised = true;
    os_changeenvironment(MemoryLimit,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.memory_limit);

    os_changeenvironment(UndefinedHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.abort_undef_handler);

    os_changeenvironment(PrefetchAbortHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.abort_prefetch_handler);

    os_changeenvironment(DataAbortHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.abort_data_handler);

    os_changeenvironment(AddressExceptionHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.abort_address_handler);

    os_changeenvironment(OtherExceptionHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.abort_exception_handler);

    os_changeenvironment(ErrorHandler,
                         &_env_error, &_kernel_error_buffer.err, &_kernel_error_buffer,
                         &oldhandlers.error_handler);

    os_changeenvironment(CallBackHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.callback_handler);

    os_changeenvironment(EscapeHandler,
                         &_env_escape, ENV_READ, ENV_READ,
                         &oldhandlers.escape_handler);

    os_changeenvironment(EventHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.event_handler);

    os_changeenvironment(ExitHandler,
                         &_env_exit, ENV_READ, ENV_READ,
                         &oldhandlers.exit_handler);

    os_changeenvironment(UpCallHandler,
                         ENV_READ, ENV_READ, ENV_READ,
                         &oldhandlers.upcall_handler);
}

/*************************************************** Gerph *********
 Function:      _env_restore
 Description:   Restore parent program environment handlers
 Parameters:    none
 Returns:       none
 ******************************************************************/
void _env_restore(void)
{
    if (!env_initialised)
        return;
    env_initialised = false;
    os_changeenvironment(MemoryLimit,
                         oldhandlers.memory_limit,
                         oldhandlers.memory_unused_r2,
                         oldhandlers.memory_unused_r3,
                         NULL);

    os_changeenvironment(UndefinedHandler,
                         oldhandlers.abort_undef_handler,
                         oldhandlers.abort_undef_unused_r2,
                         oldhandlers.abort_undef_unused_r3,
                         NULL);

    os_changeenvironment(PrefetchAbortHandler,
                         oldhandlers.abort_prefetch_handler,
                         oldhandlers.abort_prefetch_unused_r2,
                         oldhandlers.abort_prefetch_unused_r3,
                         NULL);

    os_changeenvironment(DataAbortHandler,
                         oldhandlers.abort_data_handler,
                         oldhandlers.abort_data_unused_r2,
                         oldhandlers.abort_data_unused_r3,
                         NULL);

    os_changeenvironment(AddressExceptionHandler,
                         oldhandlers.abort_address_handler,
                         oldhandlers.abort_address_unused_r2,
                         oldhandlers.abort_address_unused_r3,
                         NULL);

    os_changeenvironment(OtherExceptionHandler,
                         oldhandlers.abort_exception_handler,
                         oldhandlers.abort_exception_unused_r2,
                         oldhandlers.abort_exception_unused_r3,
                         NULL);

    os_changeenvironment(ErrorHandler,
                         oldhandlers.error_handler,
                         oldhandlers.error_workspace,
                         (intptr_t)oldhandlers.error_buffer,
                         NULL);

    os_changeenvironment(CallBackHandler,
                         oldhandlers.callback_handler,
                         oldhandlers.callback_workspace,
                         oldhandlers.callback_registers,
                         NULL);

    os_changeenvironment(EscapeHandler,
                         oldhandlers.escape_handler,
                         oldhandlers.escape_workspace,
                         oldhandlers.escape_unused_r3,
                         NULL);

    os_changeenvironment(EventHandler,
                         oldhandlers.event_handler,
                         oldhandlers.event_workspace,
                         oldhandlers.event_unused_r3,
                         NULL);

    os_changeenvironment(ExitHandler,
                         oldhandlers.exit_handler,
                         oldhandlers.exit_workspace,
                         oldhandlers.exit_unused_r3,
                         NULL);

    os_changeenvironment(UpCallHandler,
                         oldhandlers.upcall_handler,
                         oldhandlers.upcall_workspace,
                         oldhandlers.upcall_unused_r3,
                         NULL);
}
