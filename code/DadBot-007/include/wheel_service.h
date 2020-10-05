#ifndef WHEEL_SERVICE_H
#define WHEEL_SERVICE_H
#include <mutex>
#include <atomic>
#include <enums.h>

class wheel_service
{
    public:
        wheel_service();
        void initialise();
        void finalise();
        void Forward();
        void Stop();
        void Backward();
        void Turnright();
        void Turnleft();
        void speedSlow();
        void speedMedium();
        void speedFast();
        void speedSetPerc(float speedPerc);
        float getSpeedPerc();
        bool isGoingForward();
        void setDirectionDegree(int _newDegree);
        int getDirectionDegree();

    protected:

    private:
        void setWheelSpeed();
        // mutexes to prevent issues
        std::mutex wheelAccess;
        std::atomic<int> botSpeed; // keeps track of the bot's speed (range is theoreticall 0 to 255, but anything below 40 doesn't move the bot).

        bool botInitialised = false;
        TDirection direction = none;

        int directionDegree = 0;

};

#endif // WHEEL_SERVICE_H
