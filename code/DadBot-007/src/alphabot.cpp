#include <cmath>
#include <iostream>   // for standard I/O in C++
#include <pigpio.h>   // for handling the GPIO
#include <unistd.h>	  // this one is to make usleep() work
#include <string>

#include <alphabot.h>
/*
 * This class drives the bot - all actions applied to the bot, and events received from the bot are managed by this class.
 * it controlls service classes that handle each part of the bot. These classes are: -
 *    camera_servo_service: handles the servo motors that position the camera.
 *    distance_service:     handles the sonar (if present). The sonar is in the front at the middle of the bot.
 *    ir_remote_service:    handles ir signals received by the bot (sensor at the back - opposite side to the joystick).
 *    joystick_service :    picks up joystick clicks (there's a small joystick on the top of the bot).
 *    led_service:          controls the 4 LEDs at the bottom (back) of the bot (referenced by indexes 0, 1, 2 and 3).
 *    proximity_service:    handles the left and right ir proximity sensors in the front of the bot.
 *    tracking_service:     monitors the 5 line-tracking sensors at the bottom (front) of the bot.
 *    wheel_service:        handles moving both left and right wheels.
 *
 * Any code wanting to manage any part of the bot should do it through this singleton class.
 *  (you can make as many instances as you want - you'll just get the same instance all the time).
 *
 *
 * Raspbery Pi 4B GPIO pins. See https://pinout.xyz/pinout/pin3_gpio2#
 *
 *                       3.3v Power out  --   1    2  --  5v power in
 *                       BCM 2    (Sda)  --   3    4  --  5v power in
 *                       BCM 3    (Sdl)  --   5    6  --    Ground
 *                       BCM 4 (GpClk0)  --   7    8  --  BCM 14    (Txd)
 *                         Ground        --   9   10  --  BCM 15    (Rxd)
 *                       BCM 17          --  11   12  --  BCM 18   (Pwm0)
 *                       BCM 27          --  13   14  --    Ground
 *                       BCM 22          --  15   16  --  BCM 23
 *                       3.3v Power out  --  17   18  --  BCM 24
 *                       BCM 10  (MoSi)  --  19   20  --    Ground
 *                       BCM 9   (MiSo)  --  21   22  --  BCM 25
 *                       BCM 11  (SClk)  --  23   24  --  BCM 8     (Ce0)
 *                         Ground        --  25   26  --  BCM 7     (Ce1)
 *                       BCM 0  (ID_SD)  --  27   28  --  BCM 1   (ID_SC)
 *                       BCM 5           --  29   30  --    Ground
 *                       BCM 6           --  31   32  --  BCM 12   (Pwm0)
 *                       BCM 13  (Pwm1)  --  33   34  --    Ground
 *                       BCM 19  (MiSo)  --  35   36  --  BCM 16
 *                       BCM 26          --  37   38  --  BCM 20   (MoSi)
 *                         Ground        --  38   40  --  BCM 21   (SClk)
 *
 *
 * Alphabot2 use of pins: -
 *
 *                    -  3.3v Power out  --   1    2  --  5v power in       -   Power to Pi
 * ic2 camera servos  -  BCM 2    (Sda)  --   3    4  --  5v power in       -   Power to Pi
 * ic2 camera servos  -  BCM 3    (Sdl)  --   5    6  --    Ground          -   Power to Pi
 * Buzzer             -  BCM 4 (GpClk0)  --   7    8  --  BCM 14    (Txd)   -
 *                    -    Ground        --   9   10  --  BCM 15    (Rxd)   -
 * Infrared receiver  -  BCM 17          --  11   12  --  BCM 18   (Pwm0)   -   LED Lights
 * Ultrasound echo in -  BCM 27          --  13   14  --    Ground          -
 * Ultrasound trigger -  BCM 22          --  15   16  --  BCM 23            -   Tracking - DataOut (in)
 *                    -  3.3v Power out  --  17   18  --  BCM 24            -   Tracking - Addr (out)
 * Joystick left (C)  -  BCM 10  (MoSi)  --  19   20  --    Ground          -
 * Joystick right (B) -  BCM 9   (MiSo)  --  21   22  --  BCM 25            -   Tracking - Clock (out)
 * Joystick down (D)  -  BCM 11  (SClk)  --  23   24  --  BCM 8     (Ce0)   -   Joystick up (A)
 *                    -    Ground        --  25   26  --  BCM 7     (Ce1)   -   Joystick center
 *                    -  BCM 0  (ID_SD)  --  27   28  --  BCM 1   (ID_SC)   -
 * Tracking - CS (out)-  BCM 5           --  29   30  --    Ground          -
 * Left wheel speed   -  BCM 6           --  31   32  --  BCM 12   (Pwm0)   -   Left wheel back
 * Left wheel forward -  BCM 13  (Pwm1)  --  33   34  --    Ground          -
 * Proximity  (right) -  BCM 19  (MiSo)  --  35   36  --  BCM 16            -   Proximity  (left)
 * Right wheel speed  -  BCM 26          --  37   38  --  BCM 20   (MoSi)   -   Right wheel back
 *                    -    Ground        --  38   40  --  BCM 21   (SClk)   -   Right wheel forward
 *
 */

// Set up the PINS for access to the Alphabot
#define BUZ 4 // Buzzer.

alphabot* alphabot::myInstance()
{
    // Enforce single instance by Local Static variable
    static alphabot _botInstance;
    return &_botInstance;
}

//ctor
alphabot::alphabot() : myWheels(), myCameraServo(), myLed(), mySonar(), myLineTracker(), buzzerAccess()
{
    // set up the GPIO pins
    //  and configure ic2 servo management
	initialiseBot();

    // default speed is mid-range
    speedMedium();
}

 //dtor
alphabot::~alphabot()
{
    if (mypState->botInitialised) {
        // clean up everything.
        shutdownBot();
    }
    // wait for all the threads to finish off too.
    myThreader.joinThreads();
}

// NOTE: We can't capture the listen event[s] locally unless we use a static wrapper of some kind.
//      that's why they're driven from main.cpp
//      (I hope I'm wrong - but this is my current understanding)
void alphabot::ListenToProximity(const proximity_service::TProximityResultCallback & _callback) {
    // Start a thread to run the proximity sensor.
    myThreader.startThread([_callback] {
        proximity_service myProxService(_callback);
        myProxService.run();
    });
}

void alphabot::ListenToJoystick(const joystick_service::TJoystickResultCallback& _callback) {
    // Start a thread to run the proximity sensor.
    myThreader.startThread([_callback] {
        joystick_service myJsService(_callback);
        myJsService.run();
    });
}

void alphabot::ListenToIrRemote(const ir_remote_service::TIrResultCallback& _callback) {
    // Start a thread to isten to the IR receiver.
    myThreader.startThread([_callback] {
        ir_remote_service myIrService(_callback);
        myIrService.run();
    });
}

double alphabot::getDistance() {
    if (mypState->botActive) {
        return mySonar.GetDistance();
    } else {
        return 0;
    }
}

LineReadResult* alphabot::readLineSensors(bool whiteLine) {
    if (mypState->botActive) {
        return myLineTracker.readLine(whiteLine);
    } else {
        LineReadResult* result = new LineReadResult();
        return result;
    }
}

void alphabot::disableBot() {
    if (mypState->botActive) {
        // let everyone know that the bot is no longer active.
        mypState->botActive = false;

        // stop the bot moving (prevents any damage to the bot).
        Stop();
        usleep(100);

    }
}
/*
 * Shut down any ic2 management for any servos used by the app.
 *   Also close gpio handles.
 */
void alphabot::shutdownBot() {

    if (mypState->botInitialised) {
        mypState->botInitialised = false;
        // stop the bot moving (prevents any damage to the bot).
        Stop();
        usleep(100);

        // Close down the hardware.
        myWheels.finalise();
        myCameraServo.finalise();
        myLed.finalise();
        gpioTerminate();
    }

}

void alphabot::Beep(int timeInMs) {
    buzzerAccess.lock();
    gpioWrite(BUZ, 1);
    usleep(timeInMs);
    gpioWrite(BUZ, 0);
    buzzerAccess.unlock();
}

// Bring the bot to a halt.
void alphabot::Stop(void) {
    myWheels.Stop();
}

void alphabot::speedSetPerc(float speedPerc) {
    myWheels.speedSetPerc(speedPerc);
}
float alphabot::getSpeedPerc() {
    return myWheels.getSpeedPerc();
}

void alphabot::speedSlow() {
    myWheels.speedSlow();
}
void alphabot::speedMedium() {
    myWheels.speedMedium();
}
void alphabot::speedFast() {
    myWheels.speedFast();
}

void alphabot::Forward() {
    myWheels.Forward();
}
void alphabot::Backward() /*Backward, also used for a hard stop*/ {
    myWheels.Backward();
}
void alphabot::Turnright() /*Turn right*/ {
    myWheels.Turnright();
}
void alphabot::Turnleft() /*Turn left*/ {
    myWheels.Turnleft();
}
void alphabot::setWheelDirection(int _newDirection) {
    myWheels.setDirectionDegree(_newDirection);
}
int alphabot::getWheelDirection() {
    return myWheels.getDirectionDegree();
}
bool alphabot::isGoingForward() {
    return myWheels.isGoingForward();
}

void alphabot::CameraCentreBoth() /* This one centers both vertical and horizontal servos */ {
    myCameraServo.CameraCentreBoth();
}
void alphabot::CameraLeft() /* This one takes the horizontal servo to the left */ {
    myCameraServo.CameraLeft();
}
void alphabot::CameraRight() /* This one takes the horizontal servo to the right */ {
    myCameraServo.CameraRight();
}
void alphabot::CameraDown() {
    myCameraServo.CameraDown();
}

void alphabot::ledSetBrightness(int nBrightness) {
    myLed.setBrightness(nBrightness);
}
void alphabot::ledSetColour(int ledNo, uint32_t colour) {
    myLed.setColour(ledNo, colour);
}
void alphabot::ledSetAllColour(uint32_t colour0, uint32_t colour1, uint32_t colour2, uint32_t colour3) {
    myLed.setAllColour(colour0, colour1, colour2, colour3);
}
void alphabot::ledSetSameOnAll(uint32_t colour) {
    myLed.setSameOnAll(colour);
}



/*
 * Set up the GPIO pins for AlphaBot2
 *   (also set up links to the camera's servo motors)
 */
void alphabot::initialiseBot() {
    int Init;

    Init = gpioInitialise();
    if (Init < 0) {
        // pigpio initialisation failed
        printf("Pigpio initialisation failed. Error code:  %d\n", Init);
        exit(Init);
    } else {
        // pigpio initialised okay
        printf("Pigpio initialisation OK. Return code:  %d\n", Init);
    }

    // Set the pin for the Buzzer.
    gpioSetMode(BUZ, PI_OUTPUT);

    // Initialise the wheels - must happen after the gpioInitialise
	myWheels.initialise();

    // Initialise the camera servos - must happen after the gpioInitialise
	myCameraServo.initialise();

    // Initialise the LEDs
	myLed.initialise();

    // Initialise the Sonar
	mySonar.initialise();

    // Initialise the Line Tracker
	myLineTracker.initialise();

    mypState->botInitialised = true;
    mypState->botActive = true;

}
