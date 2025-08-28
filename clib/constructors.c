/*******************************************************************
 * File:        constructors
 * Purpose:     Invocation of the constructors and destructors
 * Author:      Gerph
 * Date:        18 Aug 2025
 ******************************************************************/

#include <stdbool.h>
#include <stdint.h>

typedef void (*ctordtor_func_t [])(void);

extern ctordtor_func_t __InitsStart, __InitsEnd;
extern ctordtor_func_t __FinalsStart, __FinalsEnd;

static bool constructed = false;

/*************************************************** Gerph *********
 Function:      __constructors_init
 Description:   Invoke all of the constructor functions
 Parameters:    none
 Returns:       none
 ******************************************************************/
void __constructors_init(void)
{
    int i;
    int nfuncs = (__InitsEnd - __InitsStart);

    if (constructed)
        return;

    /* We call the constructor functions in forwards order */
    for (i = 0; i < nfuncs; i++)
    {
        __InitsStart[i] ();
    }
    constructed = true;
}


/*************************************************** Gerph *********
 Function:      __constructors_init
 Description:   Invoke all of the destructor functions
 Parameters:    none
 Returns:       none
 ******************************************************************/
void __constructors_final(void)
{
    int i;
    int nfuncs = (__FinalsEnd - __FinalsStart);

    if (!constructed)
        return;

    constructed = false;

    /* We call the destructor functions in reverse order */
    for (i = nfuncs - 1; i >= 0; i--)
    {
        __FinalsStart[i] ();
    }
}
