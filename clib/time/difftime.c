
#include <time.h>

/*************************************************** Gerph *********
 Function:      difftime
 Description:   Calculate the seconds difference between s and e
 Parameters:    e = end time
                s = start time
 Returns:       seconds between times
 ******************************************************************/
double difftime(time_t e, time_t s)
{
    return (double)(e - s);
}
