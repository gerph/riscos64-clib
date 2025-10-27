/* From: https://gitlab.com/_targz/archiesdk/-/blob/master/SDK/src/libc/time/mktime.c */
/* Modified for use with RISC OS 64 */

#include <time.h>
#include <stdbool.h>
#include <stdint.h>


bool _isleap(uint32_t yr) {
    return yr % 400 == 0 || (yr % 4 == 0 && yr % 100 != 0);
}

uint32_t __months_to_days(uint32_t month) {
    return (month * 3057 - 3007) / 100;
}

int32_t __years_to_days(uint32_t yr) {
    return yr * 365L + yr / 4 - yr / 100 + yr / 400;
}

int32_t __ymd_to_scalar(uint32_t yr, uint32_t mo, uint32_t day) {
    int32_t scalar;

    scalar = day + __months_to_days(mo);
    if (mo > 2)                         /* adjust if past February */
        scalar -= _isleap(yr) ? 1 : 2;
    yr--;
    scalar += __years_to_days(yr);
    return (scalar);
}

time_t mktime(struct tm* timeptr){

    time_t tt;

    tt = __ymd_to_scalar(timeptr->tm_year + 1900, timeptr->tm_mon + 1, timeptr->tm_mday) - __ymd_to_scalar(1970, 1, 1);
    tt = tt * 24 + timeptr->tm_hour;
    tt = tt * 60 + timeptr->tm_min;
    tt = tt * 60 + timeptr->tm_sec;

    *timeptr = *gmtime(&tt);

    return tt;

}
