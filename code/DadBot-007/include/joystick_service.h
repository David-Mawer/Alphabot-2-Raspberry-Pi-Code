#ifndef JOYSTICK_SERVICE_H
#define JOYSTICK_SERVICE_H
#include <enums.h>
#include <atomic>
#include <functional>

class joystick_service
{
    public:
        typedef std::function<void(const TDirection)> TJoystickResultCallback;
        joystick_service(TJoystickResultCallback);
        void run();

    protected:

    private:
        TJoystickResultCallback m_result_callback = nullptr;
};

#endif // JOYSTICK_SERVICE_H
