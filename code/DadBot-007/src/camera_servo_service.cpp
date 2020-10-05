#include "camera_servo_service.h"
#include <cmath>
#include <iostream>   // for standard I/O in C++
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work
#include <string>

camera_servo_service::camera_servo_service() : servoAccess(), servo40()
{
    //ctor
}

/*
 * Initialise any ic2 management of servos for the app.
 */
void camera_servo_service::initialise() {
    float freq;
    int oldmode;
    int newmode;

    // Setting up the servo here
    servo40 = i2cOpen(1, 0x40, 0);
    if (servo40 >= 0) {
        // PI connection to I2C slave 40 OK
        std::cout << "Open I2C to camera servo slave 0x40 OK. Return code: " << servo40 <<"\n";
    } else {
        // No PI connection to I2C slave 40
        std::cout << "Open I2C to camera servo slave 0x40 OK failed. Error code: " << servo40 << "\n";
        exit(servo40);
    }

    // Setting the PCA9685 frequency, must be 50Hz for the SG-90 servos
    i2cWriteByteData(servo40, 0x00, 0x00); // first reset the PCA9685

    /* so.. 25*10^6 is 25Mhz of the internal clock,
     * 4096 is 12 bit resolution and 50hz is the wanted frequency setup
     * .. that's why freq is calculated below.
     */
    freq = (25000000.0 / (4096.0 * 50.0)) - 0.5;
    freq = (int) freq;
    // now there is a whole sequence to set up the frequency
    oldmode = i2cReadByteData(servo40, 0x00);	// get the current servo mode
    newmode = (oldmode &0x7F) | 0x10;			// sleep mode definition
    i2cWriteByteData(servo40, 0x00, newmode);	// going to sleep now
    i2cWriteByteData(servo40, 0xFE, freq);      // setting up the frequency now
    i2cWriteByteData(servo40, 0x00, oldmode);	// coming back online (to the old mode)
    usleep(5000);
    i2cWriteByteData(servo40, 0x00, oldmode | 0x80);    // final step on frequency set up

    botInitialised = true;
}

void camera_servo_service::finalise() {
    servoEndMessage();
    i2cClose(servo40);
    botInitialised = false;
}

/*
 * calculate the pulse required for the given "location": -
 * 	Location minimum is 500, maximum is 2500
 */
int camera_servo_service::calcServoPulse(int location) {
    float pulse_1;
    int result;

	if (location < 500) {
		pulse_1 = 500;
	} else if (location > 2500) {
		pulse_1 = 2500;
	} else {
        pulse_1 = location;
    }
    pulse_1 = (pulse_1 * 4096.0) / 20000.0; // PWM frequency is 50HZ - the period is 20000us
    result = (int) pulse_1;
    /*printf("Pulse == %d\n ", result);*/
    return result;
}
void camera_servo_service::servoEndMessage() {
    if (botInitialised) {
        i2cWriteByteData(servo40, 0xFD, 0x10); /* Shutting down all PWM channels */
        i2cWriteByteData(servo40, 0x00, 0x00); /* Resetting the PCA9685 last thing */
    }
}
void camera_servo_service::ServoWriteVert(int location) {
    if ((botInitialised) && (lastVertLocn != location)) {
        int pulse;
        // For the vertical servo
        pulse = calcServoPulse(location);
        i2cWriteByteData(servo40, 0x0A, 0 &0xFF); // on = 0
        i2cWriteByteData(servo40, 0x0B, 0 >> 8);
        i2cWriteByteData(servo40, 0x0C, pulse &0xFF); // off = given pulse value.
        i2cWriteByteData(servo40, 0x0D, pulse >> 8);
        lastVertLocn = location;
        usleep(50000);
    }
}
void camera_servo_service::ServoWriteHoriz(int location) {
    if ((botInitialised) && (lastHorizLocn != location)) {
        int pulse;
        // For the horizontal servo
        pulse = calcServoPulse(location);
        i2cWriteByteData(servo40, 0x06, 0 &0xFF); // on = 0
        i2cWriteByteData(servo40, 0x07, 0 >> 8);
        i2cWriteByteData(servo40, 0x08, pulse &0xFF); // off = given pulse value.
        i2cWriteByteData(servo40, 0x09, pulse >> 8);
        lastHorizLocn = location;
        usleep(50000);
    }
}

void camera_servo_service::CameraCentreBoth() /* This one centers both vertical and horizontal servos */ {
    if (botInitialised) {
        servoAccess.lock();
        ServoWriteVert(1500);
        ServoWriteHoriz(1500);
        servoEndMessage();
        servoAccess.unlock();
    }
}
void camera_servo_service::CameraLeft() /* This one takes the horizontal servo to the left */ {
    if (botInitialised) {
        servoAccess.lock();
        ServoWriteHoriz(2000);
        servoEndMessage();
        servoAccess.unlock();
    }
}
void camera_servo_service::CameraRight() /* This one takes the horizontal servo to the right */ {
    if (botInitialised) {
        servoAccess.lock();
        ServoWriteHoriz(1000);
        servoEndMessage();
        servoAccess.unlock();
    }
}

void camera_servo_service::CameraDown() {
    if (botInitialised) {
        servoAccess.lock();
        ServoWriteVert(1750);
        servoEndMessage();
        servoAccess.unlock();
    }
}
