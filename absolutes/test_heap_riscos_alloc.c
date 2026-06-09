#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_heap_implementation.h"

__heap_implementation_t *__heap_implementation = &__heap_riscos_alloc;

static int run_heap_smoke_test(void)
{
    char *block;
    char *grown;
    int *words;
    int i;

    block = malloc(32);
    if (block == NULL)
        return 1;

    memcpy(block, "allocator-smoke", 16);

    grown = realloc(block, 128);
    if (grown == NULL)
        return 2;

    if (memcmp(grown, "allocator-smoke", 16) != 0)
        return 3;

    words = calloc(8, sizeof(*words));
    if (words == NULL)
        return 4;

    for (i = 0; i < 8; ++i)
    {
        if (words[i] != 0)
            return 5;
        words[i] = i;
    }

    free(words);
    free(grown);

    return 0;
}

int main(void)
{
    int rc;

    rc = run_heap_smoke_test();
    if (rc != 0)
    {
        printf("FAIL %d\n", rc);
        return rc;
    }

    printf("PASS riscos_alloc\n");
    return 0;
}
