#ifndef sample_test_behaviour_H
#define sample_test_behaviour_H
#include <string>
#include <enums.h>
#include <thread_helper.h>
#include <mutex>
#include <atomic>
#include <behaviour_base.h>

class sample_test_behaviour: public behaviour_base
{
    public:
        sample_test_behaviour();
        // override of behaviour_base class interface ... Begin
        void onBotReady();
        void onBTInput(std::string);
        void onProximityCheck(bool bLeft, bool bRight);
        void onJoystickEvent(TDirection);
        void onIrEvent(int);
        // override of behaviour_base class interface ... End

    protected:

    private:
        thread_helper myThreader = thread_helper();
        bool stopIfTooClose(bool);
        void moveInDirection(TDirection direction);
        double distance = 0;
};

#endif // sample_test_behaviour_H
