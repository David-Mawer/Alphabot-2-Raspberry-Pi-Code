#include "sample_test_behaviour.h"
#include <string>
#include <iostream>   // for standard I/O in C++
#include <alphabot.h>
#include <unistd.h>	  // this one is to make usleep() work
#include <atomic>
#include <enums.h>
#include <appState.h>
#include <LineReadResult.h>
#include <opencv2/opencv.hpp>   // OpenVC libraries
#include <opencv2/highgui.hpp>

sample_test_behaviour::sample_test_behaviour() {
}

////// Implementing behaviour_base ... Begin
void sample_test_behaviour::onBotReady() {
    alphabot* mypBot = alphabot::myInstance();
    appState* mypState = appState::myInstance();

    // default to slow speed.
    mypBot->speedSlow();

    // show a cute start-up light show.
    myThreader.startThread([mypBot, mypState] {
        // run a little blink thing for 2 seconds.
        //  (we're blinking 5 times a second, so need to blink 10 times.
        int blinkNo = 0;

        int i;
        ws2811_led_t colors_rgb[8] =
        {
            Red,
            Orange,
            Yellow,
            LightBlue   // pink
        };
        int dotspos[4] = { 0, 1, 2, 3 };
        uint32_t led_strip[4];

        try {
            while ((mypState->isBotActive()) && (blinkNo < 10) && (mypState->isBotActive()))
            {
                blinkNo++;
                for (i = 0; i < 4; i++)
                {
                    dotspos[i]++;
                    if (dotspos[i] > 3)
                    {
                        dotspos[i] = 0;
                    }

                    led_strip[dotspos[i]] = colors_rgb[i];
                }

                if (mypState->isBotActive()) {
                    mypBot->ledSetAllColour(led_strip[0], led_strip[1], led_strip[2], led_strip[3]);
                }

                // 5 frames /sec
                usleep(1000000 / 5);
            }
            mypBot->ledSetBrightness(0);
            mypBot->ledSetSameOnAll(Black);
        } catch (std::exception& e) {
            std::cout << "led_service initalse error: -\n";
            std::cout << e.what() << "\n";
        }

    });

    // Variables for testing the camera
    cv::VideoCapture myVideo(0);
    cv::Mat thisFrame;
    bool haveVideo = myVideo.isOpened();

    if (!haveVideo) {
        std::cout << "Error opening video stream or file\n";
    }

    // loop to implement behaviour logic
    while (mypState->isBotActive()) {

        // Check the sonar - stop if too close to something.
        distance = mypBot->getDistance();
        stopIfTooClose(false);

/*        // check the line following sensors
        LineReadResult* myData = mypBot->readLineSensors(false);
        // Testing: Do something with the data.
        std::cout << "[";
        for (int i = 0; i < 5; i++) {
            std::cout << myData->calibratedReadResult[i] << " ";
        }
        std::cout << "]\n";
*/

        // TODO: Decide how the camera service is going to work.
        //          and then move the camera monitoring logic into that service.
        if (haveVideo && false) {
            // Check the Camera.
            myVideo.read(thisFrame);
            cv::imshow("DadBot-007 Cam", thisFrame);
            cv::waitKey(1);
        }

        usleep(10000); // wait for 100th of a second.
    }

    // wait for all my threads to finish off..
    myThreader.joinThreads();

}

void sample_test_behaviour::onBTInput(std::string psBuff) {
    alphabot* mypBot = alphabot::myInstance();

    if (psBuff == "0*") {
        // Stop
        moveInDirection(none);
    } else if (psBuff == "1*") {
        // Forward
        moveInDirection(forward);
    } else if (psBuff == "2*") {
        // Right
        moveInDirection(right);
    } else if (psBuff == "3*") {
        // Back
        moveInDirection(back);
    } else if (psBuff == "4*") {
        // Left
        moveInDirection(left);
    } else if (psBuff == "10") {
        // "Nothing" command
    } else if (psBuff == "11") {
        // Low Speed
        mypBot->speedSlow();
    } else if (psBuff == "12") {
        // Medium Speed
        mypBot->speedMedium();
    } else if (psBuff == "13") {
        // High Speed
        mypBot->speedFast();
    } else {
        // unrecognised command - show it on the console.
        std::cout << "un-configured bt-command: " << psBuff << "\n";
    }

}

void sample_test_behaviour::onJoystickEvent(TDirection direction) {
    // Copy the input parameters to local variables to prevent
    moveInDirection(direction);
}


void sample_test_behaviour::onProximityCheck(bool bLeft, bool bRight) {
    alphabot* mypBot = alphabot::myInstance();

    // Turn the camera towards the IR sensor that detects something: BEGIN
    if (bRight && !bLeft) {
        mypBot->CameraRight();
    } else if (bLeft && !bRight) {
        mypBot->CameraLeft();
    } else {
        mypBot->CameraCentreBoth();
    }
    // Turn the camera towards the IR sensor that detects something: END

}

void sample_test_behaviour::onIrEvent(int nInput) {
    alphabot* mypBot = alphabot::myInstance();

    float mySpeed = 0;

    switch(nInput) {
    case 94:
        // top-right corner
        mypBot->setWheelDirection(20);
        break;
    case 12:
        // top-left corner
        mypBot->setWheelDirection(340);
        break;
    case 74:
        // bottom-right corner
        mypBot->setWheelDirection(160);
        break;
    case 66:
        // bottom-left corner
        mypBot->setWheelDirection(200);
        break;
    case -99:
        moveInDirection(none);
        break;
    case 24:
        moveInDirection(forward);
        break;
    case 8:
        moveInDirection(left);
        break;
    case 90:
        moveInDirection(right);
        break;
    case 28: // center button
    case 82:
        moveInDirection(back);
        break;
    case 21:
        // + button
        mySpeed = mypBot->getSpeedPerc();
        mySpeed = mySpeed + 10;
        if (mySpeed > 100) {
            mySpeed = 100;
        }
        mypBot->speedSetPerc(mySpeed);
        break;
    case 7:
        // - button
        mySpeed = mypBot->getSpeedPerc();
        mySpeed = mySpeed - 10;
        if (mySpeed < 0) {
            mySpeed = 0;
        }
        mypBot->speedSetPerc(mySpeed);
        break;
    case 22:
        // slow
        mypBot->speedSlow();
        break;
    case 25:
        // medium
        mypBot->speedMedium();
        break;
    case 13:
        // fast
        mypBot->speedFast();
        break;
    case 70:
        // shut down (middle channel button)
        mypBot->disableBot();
        system("shutdown -P -t 1 now");
        break;
    default:
        // unrecognised command - show it on the console.
        std::cout << "un-configured ir-command: " << nInput << "\n";
    }
}

////// Implementing behaviour_base ... End

bool sample_test_behaviour::stopIfTooClose(bool anyDirection) {
    alphabot* mypBot = alphabot::myInstance();
    appState* mypState = appState::myInstance();

    if ((distance > 0) && (distance <= 5)) {
        if ( (mypState->isBotActive()) && ( (anyDirection) || (mypBot->isGoingForward()) ) ) {
            mypBot->Beep(60000);
            mypBot->Stop();
            // camera pointing down.
            mypBot->CameraDown();
        }

        return true;
    } else {
        return false;
    }
}

void sample_test_behaviour::moveInDirection(TDirection direction) {
    alphabot* mypBot = alphabot::myInstance();

    if (direction == none) {
        // Stop
        mypBot->Stop();
    } else if (direction == forward) {
        // Forward
        if (!stopIfTooClose(true)) {
            mypBot->Forward();
        }

    } else if (direction == right) {
        // Right
        mypBot->Turnright();
    } else if (direction == back) {
        // Back
        mypBot->Backward();
    } else if (direction == left) {
        // Left
        mypBot->Turnleft();
    }
}
