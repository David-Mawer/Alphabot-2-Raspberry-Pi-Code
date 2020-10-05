#include <cmath>
#include <iostream>   // for standard I/O in C++
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work
#include <string>
#include "wheel_service.h"

// Set up the PINS for access to the Alphabot
#define AIN1 12 // BCM pin 12, to spin (left) wheel backward
#define AIN2 13 // BCM pin 13, to spin (left) wheel forward
#define BIN1 20 // BCM pin 20, to spin (right) wheel backward
#define BIN2 21 // BCM pin 21, to spin (right) wheel forward
#define ENA 6   // BCM pin 6, enable left wheel motor (speed) PWM A
#define ENB 26  // BCM pin 26, enable right wheel motor (speed) PWM B

wheel_service::wheel_service() : wheelAccess(), botSpeed()
{
    //ctor
}

void wheel_service::initialise() {
    // Set the pins for AlphaBot2 wheel control.
    gpioSetMode(AIN1, PI_OUTPUT);
    gpioSetMode(AIN2, PI_OUTPUT);
    gpioSetMode(BIN1, PI_OUTPUT);
    gpioSetMode(BIN1, PI_OUTPUT);
    gpioSetMode(ENA, PI_OUTPUT);
    gpioSetMode(ENB, PI_OUTPUT);
    gpioSetPWMfrequency(ENA, 50000); // Set ENA to 50khz
    gpioSetPWMfrequency(ENB, 50000); // Set ENB to 50khz

    botInitialised = true;
}

void wheel_service::finalise() {
    botInitialised = false;
}

// Bring the bot to a halt.
void wheel_service::Stop(void) {
    if (botInitialised) {
        wheelAccess.lock();
        gpioWrite(AIN1, 0);
        gpioWrite(AIN2, 0);
        gpioWrite(BIN1, 0);
        gpioWrite(BIN2, 0);
        gpioPWM(ENA, 0);
        gpioPWM(ENB, 0);
        direction = none;
        wheelAccess.unlock();
    }
}

void wheel_service::speedSetPerc(float speedPerc) {
    if (speedPerc < 1) {
        Stop();
    } else {
        botSpeed = std::round(50 + ((255 - 50) * speedPerc / 100));
    }
}
float wheel_service::getSpeedPerc() {
    return (botSpeed - 50) * 100.0 / (255 - 50);
}

void wheel_service::speedSlow() {
    speedSetPerc(10);
}
void wheel_service::speedMedium() {
    speedSetPerc(49);
}
void wheel_service::speedFast() {
    speedSetPerc(90);
}

/*
 *  So - this procedure sets the wheel's direction (forward or backward for both left and right)
 *      and relative speed based on the directionDegree and current botSpeed.
 *
 *  Also - set the direction relative to the compas points.
 */
void wheel_service::setWheelSpeed() {
    directionDegree = (directionDegree % 360);
    // figure out if we're going mostly - forward / backward / left or right.
    if ((directionDegree >= 315) || (directionDegree <= 45)) {
        direction = forward;
    } else if ((directionDegree > 45) && (directionDegree < 135)) {
        direction = right;
    } else if ((directionDegree >= 135) && (directionDegree <= 225)) {
        direction = back;
    } else {
        direction = left;
    }

    // fancy equation to slow down as we turn a curner.
    float nSpeedFactor = 1.0/(3.0*45.0) * abs((directionDegree % 180) - 90.0) + 1.0/3.0;
    float nSpeedPerc = getSpeedPerc() * nSpeedFactor;

    // Now - the base wheel speed (assuming we're going forward / backward / right or left).
    // std::round(50 + ((255 - 50) * nSpeedPerc / 100));

    // set left wheel direction.
    if ((directionDegree == 225) || (directionDegree == 315)) {
        // left wheel stationary
        gpioWrite(AIN1, 0); // backward
        gpioWrite(AIN2, 0); // forward
    } else if ((directionDegree > 90) && (directionDegree < 225)) {
        // left wheel backwards
        gpioWrite(AIN1, 1); // backward
        gpioWrite(AIN2, 0); // forward
    } else if ((directionDegree >= 270) && (directionDegree < 315)) {
        // left wheel backwards
        gpioWrite(AIN1, 1); // backward
        gpioWrite(AIN2, 0); // forward
    } else {
        // left wheel forwards
        gpioWrite(AIN1, 0); // backward
        gpioWrite(AIN2, 1); // forward
    }
    // set right wheel direction.
    if ((directionDegree == 45) || (directionDegree == 135)) {
        // right wheel stationary
        gpioWrite(BIN1, 0); // backward
        gpioWrite(BIN2, 0); // forward
    } else if ((directionDegree > 45) && (directionDegree <= 90)) {
        // right wheel backward
        gpioWrite(BIN1, 1); // backward
        gpioWrite(BIN2, 0); // forward
    } else if ((directionDegree > 135) && (directionDegree < 270)) {
        // right wheel backward
        gpioWrite(BIN1, 1); // backward
        gpioWrite(BIN2, 0); // forward
    } else {
        // right wheel forward
        gpioWrite(BIN1, 0); // backward
        gpioWrite(BIN2, 1); // forward
    }

    // get wheel speed adjustment based on direction of travel.
    //  (we already have wheel direction set above )
    // now - for directions not exactly north / south / east / west
    //      we adjust one of the wheel's speed.
    float nDirectionFactor = 1.0/(45.0) * abs((directionDegree % 90) - 45.0);

    // We have the base speed, now adjust it based on the degrees.
    float leftWheelSpeedPerc = nSpeedPerc;
    float rightWheelSpeedPerc = nSpeedPerc;
    if (directionDegree > 180) {
        // Adjusting left wheel speed.
        leftWheelSpeedPerc = leftWheelSpeedPerc * nDirectionFactor;
    } else if ( (directionDegree > 0) && (directionDegree < 180)) {
        // Adjusting right wheel speed.
        rightWheelSpeedPerc = rightWheelSpeedPerc * nDirectionFactor;
    }

    // set Left wheel speed.
    gpioPWM(ENA, std::round(50 + ((255 - 50) * leftWheelSpeedPerc / 100))); /*Set ENA to a number, maximum is 255*/
    // set right wheel speed.
    gpioPWM(ENB, std::round(50 + ((255 - 50) * rightWheelSpeedPerc / 100))); /*Set ENB to number, maximum is 255*/
}

void wheel_service::setDirectionDegree(int _newDegree) {
    if (botInitialised) {
        wheelAccess.lock();
        if (botSpeed == 0) {
            speedMedium();
        }
        directionDegree = _newDegree;
        setWheelSpeed();
        wheelAccess.unlock();
    }
}
int wheel_service::getDirectionDegree() {
    return directionDegree;
}

void wheel_service::Forward(void) {
    setDirectionDegree(0);
}
void wheel_service::Backward(void) /*Backward, also used for a hard stop*/ {
    setDirectionDegree(180);
}
void wheel_service::Turnright(void) /*Turn right*/ {
    setDirectionDegree(90);
}
void wheel_service::Turnleft(void) /*Turn left*/ {
    setDirectionDegree(270);
}

bool wheel_service::isGoingForward() {
  return (direction == forward);
}

