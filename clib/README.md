# CLib for RISC OS 64-bit

This repository contains the 64-bit C library used by the RISC OS build.

## Building

Build and export the library with:

```sh
make export
```

Test absolutes live in [`../absolutes`](/riscos-source/absolutes) and can be built with:

```sh
make TARGET=<basename>
```

## Heap allocators

The library heap code supports alternative allocator backends through the
weak symbol:

```c
__heap_implementation_t *__heap_implementation;
```

If no override is supplied, the library defaults to `__heap_o1` in
[`heap/init.c`](/riscos-source/clib/heap/init.c:31).

To select a different allocator, define `__heap_implementation` in the
program being linked against the library and point it at one of the exported
allocator descriptors from [`heap/_heap_implementation.h`](/riscos-source/clib/heap/_heap_implementation.h):

```c
#include "_heap_implementation.h"

__heap_implementation_t *__heap_implementation = &__heap_riscos_alloc;
```

This override is resolved at link time and applies to the normal C allocation
entry points such as `malloc()`, `calloc()`, `realloc()` and `free()`.

### Available allocators

- `__heap_o1`: O(1) heap allocator. This is the current default.
- `__heap_riscos_alloc`: Port of the classic RISC OS allocator.
- `__heap_osheap`: Backend using `OS_Heap`.
- `__heap_accumulator`: Simple accumulator allocator.

### Examples

The allocator selection tests in [`../absolutes/test_heap_o1heap.c`](/riscos-source/absolutes/test_heap_o1heap.c)
and [`../absolutes/test_heap_riscos_alloc.c`](/riscos-source/absolutes/test_heap_riscos_alloc.c)
show how to override the heap implementation in a client binary.
