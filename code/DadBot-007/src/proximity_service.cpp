#include "proximity_service.h"
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work
#include <appState.h>

// Set up the PINS for access to the Alphabot
#define DL 16     // BCM pin 16, this is left infrared diode
#define DR 19     // BCM pin 19, this is right infrared diode

proximity_service::proximity_service(TProximityResultCallback cb)
{
    //ctor
    //m_result_callbacks.push_back(cb);
    m_result_callback = cb;

    // Reset the proximity sensors.
    gpioSetPullUpDown(DR, PI_PUD_OFF);
    gpioSetPullUpDown(DL, PI_PUD_OFF);

}

bool proximity_service::isProximity(std::string sLeftRight) {

    if (sLeftRight == "L") {
        return !gpioRead(DL);
    } else {
        return !gpioRead(DR);
    }

}

void proximity_service::run() {
    appState* mypState = appState::myInstance();

	int irDr; // IR Left sensor got something.
	int irDrPrev = 0;
	int irDl; // IR Right sensor got something.
	int irDlPrev = 0;


    while (mypState->isBotActive()) {

        irDr = isProximity("R");
        irDl = isProximity("L");

        bool sendEventInd = ((irDr != irDrPrev) || (irDl != irDlPrev));
        if (sendEventInd) {
            //for (const auto &cb : m_result_callbacks) {
            //    cb((!!irDl), (!!irDr));
            //}
            m_result_callback((!!irDl), (!!irDr));
            irDrPrev = irDr;
            irDlPrev = irDl;
        }

		usleep(10000);
	}

}
