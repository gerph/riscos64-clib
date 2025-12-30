/*******************************************************************
 * File:     callx.h
 * Purpose:  Call an arbitrary function with a set of register args
 * Author:   Gerph
 * Date:     12 Sep 2003
 ******************************************************************/

#ifndef CALLX_h
#define CALLX_h

#include "swis.h"

#ifdef __cplusplus
extern "C" {
#endif

/* _callx is nearly identical to _swix.
 * Instead of calling a SWI, however, it calls an arbitrary function.
 * The arguments are similar to those to _swix, except :
 *  - You cannot use _RETURN because there is no _call variant of this
 *    function
 *  - Registers 0-9 may be passed in.
 *  - _BLOCK(x) may NOT be used
 *  - Only registers 0-9 may be passed out
 *  - When an error occurs, the register values will be returned as per
 *    the _OUT(x) specifiers (_swix would not do this)
 *
 * The arrangement of mask is as follows:
 *
 * Bits 0 -  11: Set if R(N) is passed to the SWI.
 * Bits 22 - 31: Set if R(31-N) is output from the SWI (ie bit 31
 *               corresponds to R0, bit 22 corresponds to R9).
 * Bit 21:       Unused (was _FLAGS)
 * Bits 16 - 19: Unused (was _RETURN register)
 * Bits 12 - 15: Unused (was _BLOCK register)
 */
#if defined(__riscos) && !defined(__riscos64)
#pragma -v4
#endif
extern _kernel_oserror *_callx (void *func, void *pw, unsigned int, ...);
#if defined(__riscos) && !defined(__riscos64)
#pragma -v0
#endif


#ifdef __cplusplus
}
#endif

#endif
