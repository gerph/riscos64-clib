#include <stdint.h>
#include <stdio.h>

static uint64_t result;

uint64_t *corrupt_stack(int counter)
{
    volatile uint64_t words[2];
    words[0] = counter<<6;
    words[1] = counter<<18;
    words[2] = counter<<24;
    /* Some calculation that hopefully prevents the stores being optimised out */
    result = (result ^ words[1]) +
             (result ^ words[0]) +
             (result ^ words[2]);
    return &result;
}

int main(int argc, char *argv)
{
    uint64_t *result = corrupt_stack(argc);
    printf("Result = &%x\n", *result);
    return 0;
}
