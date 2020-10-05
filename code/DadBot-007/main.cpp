#include <iostream>   // for standard I/O in C++
#include <cstdlib>	  // for exit() definition
#include <csignal>	  // for catching exceptions e.g. control-C
#include <string>
#include <atomic>
#include <alphabot.h>
#include <appState.h>
#include <bt_handler.h>
#include <thread_helper.h>
#include <sample_test_behaviour.h>
#include <execinfo.h>
#include <unistd.h>

// only run the shutdown code once. This variable keeps track of that.
bool botShutingDown = false;
// Variable to take care of threading.
thread_helper myThreader;

// allow for many active behaviours
std::vector<behaviour_base *> myBehaviours;


void shutdownBot() {
    if (!botShutingDown) {
        // prevent recursive calls to this procedure.
        botShutingDown = true;

        // our hardware wrapper
        alphabot* mypBot = alphabot::myInstance();

        mypBot->disableBot();

        // sort out my threads - they should all pick up that the bot is disabled, and quit.
        myThreader.joinThreads();

        // shut down the hardware interfaces.
        mypBot->shutdownBot();
    }
}

/* This is the signal() function handler;
 *  Used to end the program cleanly.
 */
void signal_callback_handler(int signum) {
    printf("Caught signal %d, shutting down...\n", signum);

    shutdownBot();
    exit(2);
 }

 void segmentation_fault_handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Segmentation Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    shutdownBot();
    exit(3);
}



// pass bot & pi events on to the currently installed behaviour[s]: - Begin
void onBTInput(std::string psBuff) {
    // Copy the input parameter to a local variable to prevent
    //      segmentation faults when we pass it on to the behaviour.
    std::string sBtCmd;
    sBtCmd = psBuff;

    // pass the event on to the behaviour[s].
    for (const auto &behaviour : myBehaviours) {
        behaviour->onBTInput(sBtCmd);
    }
}
void onProximityCheck(bool bLeft, bool bRight) {
    // Copy the input parameters to local variables to prevent
    //      segmentation faults when we pass it on to the behaviour.
    bool lLeft, lRight;
    lLeft = bLeft;
    lRight = bRight;

    // pass the event on to the behaviour[s].
    for (const auto &behaviour : myBehaviours) {
        behaviour->onProximityCheck(lLeft, lRight);
    }
}
void onJoystickEvent(TDirection direction) {
    // Copy the input parameters to local variables to prevent
    //      segmentation faults when we pass it on to the behaviour.
    TDirection ldirection;
    ldirection = direction;

    // pass the event on to the behaviour[s].
    for (const auto &behaviour : myBehaviours) {
        behaviour->onJoystickEvent(ldirection);
    }
}
void onIrEvent(int nInput) {
    // Copy the input parameters to local variables to prevent
    //      segmentation faults when we pass it on to the behaviour.
    int lInput;
    lInput = nInput;

    // pass the event on to the behaviour[s].
    for (const auto &behaviour : myBehaviours) {
        behaviour->onIrEvent(lInput);
    }
}
// pass bot & pi events on to the currently installed behaviour[s]: - End



// main process - hook up all the events to the current behaviour.
//  then get the bot to run.
int main(int argc, char * argv[]) {
    // load up the behaviours
    myBehaviours.push_back(new sample_test_behaviour());
    // our hardware wrapper
    alphabot* mypBot = alphabot::myInstance();
    appState* mypState = appState::myInstance();

    // Handle interruptions gracefully (user presses break / segmentation fault)
    signal(SIGINT, signal_callback_handler);
    signal(SIGSEGV, segmentation_fault_handler);

    // center the camera
    mypBot->CameraCentreBoth();
    // listen to proximity events
    mypBot->ListenToProximity(*onProximityCheck);
    // listen to joystick events
    mypBot->ListenToJoystick(*onJoystickEvent);
    // listen for irRemote events
    mypBot->ListenToIrRemote(*onIrEvent);

    // Start a thread to monitor the Bluetooth adapter.
    // (this adapter sits on the Pi itself - so it's sitting here seperately to the alphabot singleton)
    myThreader.startThread([] {
        bt_handler myBT(onBTInput);
        myBT.run();
    });

    // Tell the behaviours that the bot is up and running
    // (each behaviour runs in it's own thread).
    for (const auto &behaviour : myBehaviours) {
        myThreader.startThread([behaviour] {
            behaviour->onBotReady();
        });
    }

    // ok - now just let the behaviours control the bot until the term singal is received.
    while (mypState->isBotActive()) {
		usleep(1000000); // hang around for 1 second.
	}

    shutdownBot();

    exit(0);
}
