/*******************************************************************
 * File:        _heap_implementation
 * Purpose:     Implementation of the heap management functions.
 * Author:      Gerph
 * Date:        2024-09-01
 ******************************************************************/

#ifndef __HEAP_IMPLEMENTATION
#define __HEAP_IMPLEMENTATION

#include <stddef.h>

typedef struct __heap_implementation_s {

    /* Initialisation of the heap, given the available size */
    void (*init)(void *base, size_t size);

    /* Allocator, to return a block of heap of the given size, or NULL if cannot */
    void *(*alloc)(size_t);

    /* Free, to release a block, of a known size */
    void (*free)(void *block, size_t size);

    /* Reallocate a previously allocated block, or NULL if cannot */
    void *(*realloc)(void *block, size_t new_size, size_t old_size);

} __heap_implementation_t;


/* Heap implementations provided by the system are listed here: */

/* Rudimentary accumulator heap */
extern __heap_implementation_t __heap_accumulator;

/* Implementation using OS_Heap */
extern __heap_implementation_t __heap_osheap;

/* Implementation using the classic RISC OS allocator */
extern __heap_implementation_t __heap_riscos_alloc;

/* O(1) heap implementation: https://github.com/pavel-kirienko/o1heap */
extern __heap_implementation_t __heap_o1; /* default */

#endif
