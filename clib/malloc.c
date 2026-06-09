/*******************************************************************
 * File:        malloc
 * Purpose:     Memory allocation on the heap
 * Author:      Gerph
 * Date:        03 Aug 2024
 ******************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "swis.h"
#include "swis_os.h"
#include "kernel/kernel-state.h"

#include "heap/init.h"
#include "heap/internal.h"

//#define DEBUG

#ifdef DEBUG
#define dprintf if (1) printf
#else
#define dprintf if (0) printf
#endif


void __heap_fatal(const char *msg)
{
    os_write0(msg);
    os_newline();
    exit(1);
}


typedef struct alloc_s {
    size_t size;
    char data[0];
} alloc_t;


#ifdef HEAP_FAIL_IS_FATAL
void __heap_alloc_failed(size_t size)
{
    char buffer[80];
#define MSG0 "OUT OF MEMORY - need &"
#define MSG1 " bytes"
    char *s;
    memcpy(buffer, MSG0, sizeof(MSG0) - 1);
    s = buffer + sizeof(MSG0) - 1;
    uint64_t need = size;
    int started = 0;
    for(int i=60; i >= 0; i-=4)
    {
        unsigned int v = (need>>i) & 15;
        if (v || started || i == 0)
        {
            started = 1;
            *s++ = "0123456789ABCDEF"[v];
        }
    }
    memcpy(s, MSG1, sizeof(MSG1));
    __heap_fatal(buffer);
}
#endif


__THROW __attribute_malloc__
void *malloc (size_t size)
{
    void *allocated = NULL;

    dprintf("malloc(%i) => ", size);
    if (size == 0)
    {
        dprintf("%p\n", NULL);
        return NULL;
    }

    if (_kernel_inmodule)
    {
        /* Allocate through OS_Module */
        _kernel_oserror *err = os_module(6, 0, size, (void**)&allocated);
        if (err)
            allocated = NULL;
        dprintf("%p\n", allocated);
        return allocated;
    }

    if (__heap_base == NULL)
        __heap_fatal("No heap memory");

    if (!__heap_inited)
    {
        dprintf("{first heap init %p -> %p (%i)}", __heap_base, __heap_end, __heap_end - __heap_base);
        HEAP_FUNC(init)(__heap_base, __heap_end - __heap_base);
        __heap_inited = 1;
    }

    /* Round to a multiple of 8 */
    size = (size + 7 + sizeof(alloc_t)) & ~7;
    allocated = HEAP_FUNC(alloc) (size);

    if (allocated) {
        alloc_t *p = (alloc_t*)allocated;
        p->size = size;
        allocated = (void*)&p->data;

        dprintf("%p\n", allocated);
        return allocated;
    }

#ifdef HEAP_FAIL_IS_FATAL
    dprintf("Allocation failed\n");
    __heap_alloc_failed(size);
#else
    dprintf("%p\n", NULL);
    errno = ENOMEM;
    return NULL;
#endif
}

void *calloc(size_t count, size_t size)
{
    size_t total = count * size;
    void *mem = malloc(total);
    if (mem)
        memset(mem, 0, total);
    return mem;
}

void free(void *block)
{
    dprintf("free(%p)\n", block);
    if (block == NULL)
        return;

    if (_kernel_inmodule)
    {
        /* Free through OS_Module */
        _kernel_oserror *err = os_module(7, block, 0, NULL);
        /* Cannot do anything with the error except flag it */
        if (err)
            errno = ENOMEM;
        return;
    }

    if (__heap_base == NULL)
        __heap_fatal("No heap memory");

    alloc_t *p = ((alloc_t *)block) - 1;
    HEAP_FUNC(free) (p, p->size);
}

void *realloc(void *block, size_t size)
{
    dprintf("realloc(%p, %i) => ", block, size);
    if (size == 0 && block == NULL)
    {
        dprintf("ignore\n");
        return NULL;
    }
    if (size == 0)
    {
        dprintf("free\n");
        free(block);
        return NULL;
    }
    if (block == NULL)
    {
        dprintf("malloc\n");
        return malloc(size);
    }

    if (_kernel_inmodule)
    {
        /* Resize through OS_Module */
        void *new_block;
        uint32_t *p = (uint32_t*)block;
        size_t old_size = p[-1] - 4;
        _kernel_oserror *err = os_module(13, block, size - old_size, &new_block);
        if (err)
            return NULL;
        return new_block;
    }

    if (__heap_base == NULL)
        __heap_fatal("No heap memory");

    size_t new_size = (size + 7 + sizeof(alloc_t)) & ~7;

    alloc_t *p = ((alloc_t *)block) - 1;
    size_t old_size = p->size;

    void *allocated = HEAP_FUNC(realloc) (p, new_size, old_size);

    if (allocated == NULL)
    {
        errno = ENOMEM;
#ifdef HEAP_FAIL_IS_FATAL
        __heap_alloc_failed(size);
#endif
    }
    else
    {
        alloc_t *p = (alloc_t*)allocated;
        p->size = new_size;
        allocated = (void*)&p->data;
    }

    dprintf("%p\n", allocated);

    return allocated;
}
