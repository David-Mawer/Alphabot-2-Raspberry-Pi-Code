#ifndef UTILS_H
#define UTILS_H
#include <sys/time.h> // for clock_gettime related

class utils
{
    public:
        static double timeDiffInSec(struct timeval _fromTime, struct timeval _toTime);

    protected:

    private:
};

#endif // UTILS_H
