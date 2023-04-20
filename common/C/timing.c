#include "./timing.h"

long calculate_time_difference(struct timespec *start, struct timespec *end)
{
    long start_nanoseconds = start->tv_sec * NANOSECONDS_IN_ONE_SECOND + start->tv_nsec;
    long end_nanoseconds = end->tv_sec * NANOSECONDS_IN_ONE_SECOND + end->tv_nsec;
    return end_nanoseconds - start_nanoseconds;
}