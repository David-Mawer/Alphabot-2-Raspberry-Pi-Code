#ifndef CAMERA_SERVO_SERVICE_H
#define CAMERA_SERVO_SERVICE_H
#include <mutex>
#include <atomic>


class camera_servo_service
{
    public:
        camera_servo_service();
        void initialise();
        void finalise();
        void ServoWriteVert(int);
        void ServoWriteHoriz(int);
        void CameraCentreBoth();
        void CameraLeft();
        void CameraRight();
        void CameraDown();

    protected:

    private:
        // mutex to prevent issues
        std::mutex servoAccess;

        // local variables
        std::atomic<int> servo40; // used in the signal_callback_handler procedure - only reason it's a global.
        int calcServoPulse(int);
        void servoEndMessage();

        // keep track of the current camera servo motor positions.
        int lastHorizLocn = -1;
        int lastVertLocn = -1;

        bool botInitialised = false;
};

#endif // CAMERA_SERVO_SERVICE_H
