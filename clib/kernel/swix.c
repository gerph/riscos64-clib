#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include "kernel.h"
#include "kernel-state.h"
#include "swis.h"

/*************************************************** Gerph *********
 Function:      _kernel_last_oserror
 Description:   Return the pointer to the last error reported by
                a _kernel function call (or a call that invokes one)
 Parameters:    none
 Returns:       pointer to error, or NULL if no error reported yet
 ******************************************************************/
_kernel_oserror *_kernel_last_oserror(void)
{
    return _last_error;
}


/*
 * Bits 0 -  9:  Set if R(N) is passed to the SWI.
 * Bits 22 - 31: Set if R(31-N) is output from the SWI (ie bit 31
 *               corresponds to R0, bit 22 corresponds to R9).
 * Bit 21:       Set if the PC (including the flags) is to be output.
 * Bits 16 - 19: Register no. to be returned from a _swi call. This is only
 *               applicable to _swi as _swix always returns either 0 or an error
 *               pointer.
 * Bit 11:       Set if a local block parameter is to be passed to the SWI
 * Bits 12 - 15: Register number for local block parameter if bit 11 set.
 */
_kernel_oserror *_swix(int swi_no, uint32_t flags, ...)
{
    _kernel_oserror *err;
    _kernel_swi_regs regs = {0};
    va_list args;
    int n;

    va_start(args, flags);

    for (n=0; n<10; n++)
    {
        if (flags & (1<<n))
            regs.r[n] = va_arg(args, uint64_t);
    }
    err = _kernel_swi(swi_no, &regs, &regs);
    if (err)
        _kernel_copyerror(err);

    for (n=0; n<10; n++)
    {
        if (flags & (1lu<<(31-n)))
        {
            uint32_t *p = va_arg(args, uint32_t *);
            *p = regs.r[n];
        }
    }
    /* FIXME: No flags returned */
    /* FIXME: No register block */
    va_end(args);

    return err;
}
