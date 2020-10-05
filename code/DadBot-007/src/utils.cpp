#include "utils.h"

double utils::timeDiffInSec(struct timeval _fromTime, struct timeval _toTime) {
     // get the result in micro-seconds, convert to float - then to seconds.
    return (_toTime.tv_usec - _fromTime.tv_usec) * 1.0 / 1000000;
}
