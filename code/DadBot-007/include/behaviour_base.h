#ifndef BEHAVIOUR_BASE_H
#define BEHAVIOUR_BASE_H

#include <string>
#include <enums.h>
#include <thread_helper.h>


class behaviour_base
{
    public:
        behaviour_base() {};
        virtual ~behaviour_base() {};
        virtual void onBTInput(std::string) = 0;
        virtual void onProximityCheck(bool bLeft, bool bRight) = 0;
        virtual void onJoystickEvent(TDirection) = 0;
        virtual void onIrEvent(int) = 0;
        virtual void onBotReady() = 0;

    protected:

    private:
};

#endif // BEHAVIOUR_BASE_H
