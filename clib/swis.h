#ifndef _SWIS_H
#define _SWIS_H

#include <stdint.h>
#include "kernel.h"

#include "swi_numbers.h"

#define _IN(i)     (1u << (i))
#define _INR(i,j)  (((1u<<(i))-1) ^ ((1u<<((j)+1))-1))
#define _OUT(i)    (1u << (31 - (i)))
#define _OUTR(i,j) ( ((1u<<(1 + (j) - (i)))-1) << (31 - (j)) )
#define _RETURN(i) ((i) << 16)
#define _C         (1U << 29)
#define _Z         (1U << 30)
#define _N         (1U << 31)

_kernel_oserror *_swix (int swi_no, uint32_t flags, ...);
uint64_t _swi(int swi_no, uint32_t flags, ...);

#endif
