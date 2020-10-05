#include "tracking_service.h"
#include <iostream>   // for standard I/O in C++
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work

// Set GPIO pins
#define CS 5        // BCM pin 5, xxx--xxxx
#define CLOCK 25    // BCM pin 25, xxx--xxx
#define ADDRESS 24  // BCM pin 24, xxx--xxx
#define DATAOUT 23  // BCM pin 23, xxx--xxx

// Code copied from the TRSensors.py without much understanding...
// ..just got it to the point that it works for now.

tracking_service::tracking_service() : trackingAccess()
{
    //ctor
    std::fill_n(calibratedMax, numSensors, 1023);

    last_value = 0;

}

// called after gpio set up.
void tracking_service::initialise() {

    gpioSetMode(CS, PI_OUTPUT);
    gpioSetMode(CLOCK, PI_OUTPUT);
    gpioSetMode(ADDRESS, PI_OUTPUT);
    gpioSetMode(DATAOUT, PI_INPUT);
    gpioSetPullUpDown(DATAOUT, PI_PUD_UP);

}

/*
    The values returned are a measure of the reflectance in abstract units,
    with higher values corresponding to lower reflectance (e.g. a black
    surface or a void).
*/
void tracking_service::analogRead() {
    int value[6] = {0}; // used to keep track of the IR scan result.
    int i, j; // used for loop counters.

    trackingAccess.lock();

    try {

        for(j = 0; j < numSensors + 1; j++) {
            gpioWrite(CS, 0);

            for (i = 0; i < 4; i++) {
                // sent 4-bit ADDRESS
                if (((j) >> (3 - i)) & 0x01) {
                    gpioWrite(ADDRESS, 1);
                } else {
                    gpioWrite(ADDRESS, 0);
                }

                //read MSB 4-bit data
                value[j] <<= 1;
                if ( gpioRead(DATAOUT) ) {
                    value[j] |= 0x01;
                }
                gpioWrite(CLOCK, 1);
                gpioWrite(CLOCK, 0);
            }

            for (i = 0; i < numSensors + 1; i++) {
                // read LSB 8-bit data
                value[j] <<= 1;
                if ( gpioRead(DATAOUT) ) {
                    value[j] |= 0x01;
                }
                gpioWrite(CLOCK, 1);
                gpioWrite(CLOCK, 0);
            }

            usleep(100);
            gpioWrite(CS, 1);

        }
    } catch (std::exception& e) {
        std::cout << "tracking_service analogRead error: -\n";
        std::cout << e.what() << "\n";
    }
    trackingAccess.unlock();

    for(j = 0; j < numSensors; j++) {
        analogReadResult[j] = value[j + 1];
    }

}

/*
    Reads the sensors 10 times and uses the results for
    calibration.  The sensor values are not returned; instead, the
    maximum and minimum values found over time are stored internally
    and used for the calibratedRead() method.
*/
void tracking_service::calibrate() {
    int max_sensor_values[numSensors] = { 0 };
    int min_sensor_values[numSensors] = { 0 };
    int i, j; // used for loop counters.

    for(j = 0; j < 10; j++) {

        analogRead();

        for(i = 0; i < numSensors; i++) {
            // set the max we found THIS time
            if( (j == 0) || (max_sensor_values[i] < analogReadResult[i])) {
                max_sensor_values[i] = analogReadResult[i];
            }
            // set the min we found THIS time
            if( (j == 0) || (min_sensor_values[i] > analogReadResult[i])) {
                min_sensor_values[i] = analogReadResult[i];
            }
        }

    }

    // record the min and max calibration values
    for(i = 0; i < numSensors; i++) {
        if(min_sensor_values[i] > calibratedMin[i]){
            calibratedMin[i] = min_sensor_values[i];
        }
        if(max_sensor_values[i] < calibratedMax[i]){
            calibratedMax[i] = max_sensor_values[i];
        }
   }

}

/*
    Returns values calibrated to a value between 0 and 1000, where
    0 corresponds to the minimum value read by calibrate() and 1000
    corresponds to the maximum value.  Calibration values are
    stored separately for each sensor, so that differences in the
    sensors are accounted for automatically.
*/
void tracking_service::calibratedRead() {
    int i; // used for loop counters.

    //read the needed values
    analogRead();

    // check the calibration's OK
    for(i = 0; i < numSensors; i++) {
        if(analogReadResult[i] < calibratedMin[i]){
            calibratedMin[i] = analogReadResult[i];
        }
        if(analogReadResult[i] > calibratedMax[i]){
            calibratedMax[i] = analogReadResult[i];
        }
    }

    for(i = 0; i < numSensors; i++) {
        int calibratedValue = 0; // used to store the calibrated value calculated from the analog value (for each sensor).

        int denominator = calibratedMax[i] - calibratedMin[i];

        if(denominator != 0) {
            calibratedValue = (analogReadResult[i] - calibratedMin[i] ) * 1000 / denominator;
        }

        if(calibratedValue < 0) {
            calibratedValue = 0;
        } else if (calibratedValue > 1000) {
            calibratedValue = 1000;
        }

        calibratedReadResult[i] = calibratedValue;
    }
}

LineReadResult* tracking_service::readLine(bool whiteLine) {
    LineReadResult* result = new LineReadResult();
    calibratedRead();
    int avg = 0;
    int sum = 0;
    bool on_line = false;
    int i; // used for loop counters.
    int calibratedValue = 0;

    for(i = 0; i < numSensors; i++) {

        calibratedValue = calibratedReadResult[i];

        if (whiteLine) {
            calibratedValue = 1000 - calibratedValue;
        }

        // keep track of whether we see the line at all
        on_line = (calibratedValue > 200);

        // only average in values that are above a noise threshold
        if(calibratedValue > 50) {
            avg += calibratedValue * (i * 1000);  // this is for the weighted total,
            sum += calibratedValue;               // this is for the denominator
        }
    }

    if (on_line) {
        // If it last read to the left of center, return 0.
        if( last_value < (numSensors - 1)*1000/2 ) {
            // print("left")
            last_value = 0;
        }
        // If it last read to the right of center, return the max.
        else {
            //print("right")
            last_value = (numSensors - 1)*1000;
        }
    } else {
        if (sum != 0) {
            last_value = avg / sum;
        } else {
            last_value = 0;
        }
    }

    // Copy the results to the parameter.
    result->last_value = last_value;
    for (i = 0; i < numSensors; i++) {
        result->analogReadResult[i] = analogReadResult[i];
        result->calibratedReadResult[i] = calibratedReadResult[i];
        result->lineReadResult[i] = lineReadResult[i];
    }

    return result;
}
