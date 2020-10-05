#include "distance_service.h"
#include <iostream>   // for standard I/O in C++
#include <unistd.h>	  // this one is to make usleep() work
#include <pigpio.h>   // for handling the GPIO
#include <sys/time.h> // for clock_gettime related
#include <utils.h>
#include <alphabot.h>

// Set GPIO pins
#define US_TRIG 22  // BCM pin 22, sending ultrasound cry
#define US_ECHO 27  // BCM pin 27, receiving ultrasound

distance_service::distance_service()
{
    //ctor
}

// called after gpio set up.
void distance_service::initialise() {
    // Set the pins for the Sonar module.
    gpioSetMode(US_ECHO, PI_INPUT);
    gpioSetMode(US_TRIG, PI_OUTPUT);
}

double distance_service::GetDistance() {
    struct timeval t1, t2, t3;
    double start_wait_seconds = 0, pulse_time_seconds = 0, distance = 0;

    // mark the time before anything happened.
    gettimeofday( &t3, NULL);

    // Trigger the Sonar pulse.
    gpioWrite(US_TRIG, 1);
    usleep(15);
    gpioWrite(US_TRIG, 0);

    // wait for the pulse to start - not more than 5/100th of a second.
    // store the start time in t1
    while (!gpioRead(US_ECHO)) {
        gettimeofday( &t1, NULL);
        start_wait_seconds = utils::timeDiffInSec(t3, t1);
        if (start_wait_seconds > 0.06)
            break;
    }

    // wait for the pulse to end (if it didn't start - then just record the timeout).
    // store the end-time in t2
    while (gpioRead(US_ECHO)) {
        gettimeofday( &t2, NULL);
        if (start_wait_seconds > 0.06)
            break;
    }

    pulse_time_seconds = utils::timeDiffInSec(t1, t2);
    // calculate the distance based on the echo time
    if ((start_wait_seconds < 0.06) && (pulse_time_seconds < 0.06)) {
        // distance = time * speed
        // speed = 17150 = 1/2 speed of sound (in centimeters/second)
        //  (need 1/2 because pulse duration is time for sound to get there and back)
        distance = pulse_time_seconds * 17150;
    }

    return distance;

}
