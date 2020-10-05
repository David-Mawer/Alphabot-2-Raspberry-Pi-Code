#ifndef alphabot_H
#define alphabot_H
#include <proximity_service.h>
#include <joystick_service.h>
#include <ir_remote_service.h>
#include <distance_service.h>
#include <wheel_service.h>
#include <camera_servo_service.h>
#include <tracking_service.h>
#include <led_service.h>
#include <LineReadResult.h>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread_helper.h>
#include <appState.h>

class alphabot
{
    public:
        static alphabot* myInstance();
        void disableBot();
        void shutdownBot();

        void Forward();
        void Stop();
        void Backward();
        void Turnright();
        void Turnleft();
        void setWheelDirection(int _newDirection);
        int getWheelDirection();

        void CameraCentreBoth();
        void CameraLeft();
        void CameraRight();
        void CameraDown();

        void speedSlow();
        void speedMedium();
        void speedFast();
        void speedSetPerc(float speedPerc);
        float getSpeedPerc();

        void Beep(int);

        bool isGoingForward();

        void ledSetBrightness(int);
        void ledSetColour(int ledNo, uint32_t colour);
        void ledSetAllColour(uint32_t colour0, uint32_t colour1, uint32_t colour2, uint32_t colour3);
        void ledSetSameOnAll(uint32_t);
        double getDistance();
        LineReadResult* readLineSensors(bool whiteLine);

        void ListenToProximity(const proximity_service::TProximityResultCallback&);
        void ListenToJoystick(const joystick_service::TJoystickResultCallback&);
        void ListenToIrRemote(const ir_remote_service::TIrResultCallback&);

    protected:

    private:
        // Construct and analyze it as private, prohibit external structure and Deconstruction
        alphabot();
        virtual ~alphabot();

        // Prohibit external replication constructs
        alphabot(const alphabot &bot);
        // Prohibit external assignment operations
        const alphabot &operator=(const alphabot &bot);

        void initialiseBot();

        // Variable to take care of threading.
        thread_helper myThreader = thread_helper();

        wheel_service myWheels;
        camera_servo_service myCameraServo;
        led_service myLed;
        distance_service mySonar;
        tracking_service myLineTracker;

        // mutex to prevent issues
        std::mutex buzzerAccess;

        appState* mypState = appState::myInstance();

};

#endif // alphabot_H
