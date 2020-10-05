#ifndef TRACKING_SERVICE_H
#define TRACKING_SERVICE_H

#include <atomic>
#include <functional>
#include <mutex>
#include <LineReadResult.h>

class tracking_service
{
    public:
        tracking_service();
        void initialise();
        LineReadResult* readLine(bool whiteLine);

    protected:

    private:
        int numSensors = 5;
        // mutexes to prevent issues
        std::mutex trackingAccess;
        int analogReadResult[5] = { 0 };
        void analogRead();
        int calibratedMin[5] = { 0 };
        int calibratedMax[5] = { 0 };
        void calibrate();
        int calibratedReadResult[5] = { 0 };
        void calibratedRead();
        int lineReadResult[5] = { 0 };
        int last_value = 0;

};

#endif // TRACKING_SERVICE_H
