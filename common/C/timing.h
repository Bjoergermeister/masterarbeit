#include <time.h>

#define MILLISECONDS_IN_ONE_SECOND 1000
#define NANOSECONDS_IN_ONE_MILLISECOND 1000000
#define NANOSECONDS_IN_ONE_SECOND 1000000000

long calculate_time_difference(struct timespec *start, struct timespec *end);