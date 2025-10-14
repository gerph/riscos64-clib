/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1990, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Posix rand_r function added May 1999 by Wes Peters <wes@softweyr.com>.
 */
/* From: https://github.com/freebsd/freebsd-src/blob/main/lib/libc/stdlib/rand.c */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "random/random.h"

/*
 * Implement rand(3), the standard C PRNG API, using the non-standard but
 * higher quality random(3) implementation and the same size 128-byte state
 * LFSR as the random(3) default.
 *
 * It turns out there are portable applications that want a PRNG but are too
 * lazy to use better-but-nonstandard interfaces like random(3), when
 * available, and too lazy to import higher-quality and faster PRNGs into their
 * codebase (such as any of SFC, JSF, 128-bit LCGs, PCG, or Splitmix64).
 *
 * Since we're stuck with rand(3) due to the C standard, we can at least have
 * it produce a relatively good PRNG sequence using our existing random(3)
 * LFSR.  The random(3) design is not particularly fast nor compact, but it has
 * the advantage of being the one already in the tree.
 */
static struct __random_state *rand3_state;
static bool rand3_state_initialised = false;

static void
initialize_rand3(void)
{
    int error;

    rand3_state = _fbsd_allocatestate(TYPE_3);
    error = _fbsd_initstate_r(rand3_state, 1, rand3_state->rst_randtbl, BREAK_3);
    assert(error == 0);
    rand3_state_initialised = true;
}

int
rand(void)
{
    if (!rand3_state_initialised)
        initialize_rand3();
    return ((int)_fbsd_random_r(rand3_state));
}

void
srand(unsigned seed)
{
    if (!rand3_state_initialised)
        initialize_rand3();
    _fbsd_srandom_r(rand3_state, seed);
}
