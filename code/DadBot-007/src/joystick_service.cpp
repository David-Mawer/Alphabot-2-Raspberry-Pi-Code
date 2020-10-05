#include <iostream>   // for standard I/O in C++
#include "joystick_service.h"
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work
#include <enums.h>
#include <appState.h>

// Set up the PINS for access to the Alphabot
#define JS_CTR 7    // BCM pin 7, this is joystick click in center.
#define JS_FWD 8    // BCM pin 8, this is joystick click in up.
#define JS_RT 9     // BCM pin 9, this is joystick click in right.
#define JS_LT 10    // BCM pin 10, this is joystick click in left.
#define JS_BCK 11   // BCM pin 11, this is joystick click in down.

joystick_service::joystick_service(TJoystickResultCallback cb)
{
    //ctor
    m_result_callback = cb;

    // set the gpio pins wired to the joysticter as inputs
    gpioSetMode(JS_CTR, PI_INPUT);
    gpioSetMode(JS_FWD, PI_INPUT);
    gpioSetMode(JS_RT, PI_INPUT);
    gpioSetMode(JS_LT, PI_INPUT);
    gpioSetMode(JS_BCK, PI_INPUT);

    // Reset the listening on these pins.
    gpioSetPullUpDown(JS_CTR, PI_PUD_UP);
    gpioSetPullUpDown(JS_FWD, PI_PUD_UP);
    gpioSetPullUpDown(JS_RT, PI_PUD_UP);
    gpioSetPullUpDown(JS_LT, PI_PUD_UP);
    gpioSetPullUpDown(JS_BCK, PI_PUD_UP);


}

void joystick_service::run() {
    appState* mypState = appState::myInstance();

    bool sendEvent;
    while (mypState->isBotActive()) {

        sendEvent = true;
        TDirection myDir;

        if (gpioRead(JS_CTR) == 0) {
            myDir = none;
        } else if (gpioRead(JS_FWD) == 0) {
            myDir = forward;
        } else if (gpioRead(JS_RT) == 0) {
            myDir = right;
        } else if (gpioRead(JS_LT) == 0) {
            myDir = left;
        } else if (gpioRead(JS_BCK) == 0) {
            myDir = back;
        } else {
            sendEvent = false;
        }

        if (sendEvent) {
            if (m_result_callback != nullptr) {
                m_result_callback(myDir);
            }
        }

		usleep(10000);
	}

}
