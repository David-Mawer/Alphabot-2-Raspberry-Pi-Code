#include <iostream>   // for standard I/O in C++
#include "ir_remote_service.h"
#include <string>
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work
#include <sys/time.h> // for clock_gettime related
#include <utils.h>
#include <appState.h>

#define IR_IN 17   // BCM pin 17, this is infrared receiver.

ir_remote_service::ir_remote_service(TIrResultCallback cb)
{
    //ctor
    m_result_callback = cb;

    // set the gpio pin wired to the infrared receiver.
    gpioSetMode(IR_IN, PI_INPUT);

}

int ir_remote_service::getKey() {
    int nResult = -99;
    int count, idx, cnt;
    int data[ ] = { 0,0,0,0 };

    if (gpioRead(IR_IN) == 0) {
        // figure out what the key is.
        count = 0;
        while ((gpioRead(IR_IN) == 0) && (count < 200)) { // 9ms
            count ++;
            usleep(45);
        }

        if(count < 10) {
            return nResult;
        }

        count = 0;
        while ((gpioRead(IR_IN) == 1) && (count < 100)) { // 4.5ms
            count ++;
            usleep(45);
        }

        idx = 0;
        cnt = 0;
        for(int i = 0; i < 32; i++)
        {
            count = 0;
            while ((gpioRead(IR_IN) == 0) && (count < 12)){ // .56ms
                count ++;
                usleep(45);
            }

            count = 0;
            while ((gpioRead(IR_IN) == 1) && (count < 37)) { // 0.56ms => 0
                                                             // 1.69ms => 1
                count ++;
                usleep(45);
            }

            if (count > 7) {
                data[idx] |= 1<<cnt;
            }
            if (cnt == 7) {
                cnt = 0;
                idx ++;
            } else {
                cnt ++;
            }
        }
        // std::cout << data << "\n";

        if (((data[0]+data[1]) == 0xFF) && ((data[2]+data[3]) == 0xFF)) {  // check
            nResult = data[2];
            lastKeyPress = nResult;
        } else {
            nResult = 10000 + lastKeyPress;
        }

    }

    return nResult;
}

void ir_remote_service::run() {
    appState* mypState = appState::myInstance();

    int nKeyNum;
    int nLastKeySent = -99;
    bool bStopSent = true;
    struct timeval tStart, tCheck;

    while (mypState->isBotActive()) {
        nKeyNum = getKey();

        if (nKeyNum == -99) {
            // Got a -99 (no key)
            // if timer lapsed; send -99
            if (!bStopSent) {
                gettimeofday( &tCheck, NULL);
                double timeDiff = utils::timeDiffInSec(tStart, tCheck);
                if (timeDiff >= 0.1) {
                    if (m_result_callback != nullptr) {
                        m_result_callback(nKeyNum);
                    }
                    nLastKeySent = nKeyNum;
                    bStopSent = true;
                }
            }
        } else {
            bStopSent = false;
            // Got a first key-press, or a repeate key-press.
            if (nKeyNum < 10000) {
                // Got a key - send it.
                // send the event.
                if (m_result_callback != nullptr) {
                    m_result_callback(nKeyNum);
                }
                nLastKeySent = nKeyNum;
                gettimeofday( &tStart, NULL);
            } else if ((nKeyNum % 10000) == (nLastKeySent % 10000)) {
                // Got a repeat - reset the timer, and leave it at that.
                gettimeofday( &tStart, NULL);
            }
        }

		usleep(100);
	}
}

