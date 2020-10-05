
# Alphabot-2-Raspberry-Pi-Code

This is a C++ program to manage an Alphabot 2 using a Raspberry Pi 4.

few months ago I got an AlphaBot 2 (the one that comes with the Raspberry Pi 4).

It was really cool learning how to work with the bot - most samples were written in Python (which is easy to get into).

Some googling told me that it was a good idea to use C or C++ (especially if I know that performance is going to be an issue later on in my coding).

There were very few examples in C or C++, so I set about to make one.

So here's the result of my experimentation. It's a program written in C++ that handles all the bits of hardware that makes up an AlphaBot 2.



### Instructions to get the code compiled: -
1. Install libopencv-dev using the command
  **sudo apt install libopencv-dev**
  
2. copy the external libs to your local lib folder by running
  **./code/DadBot-007/copy-lib-local.sh**

3. load up the project file in **code/DadBot-007/DadBot-007.cbp** using Code::Blocks - and build the **Release** version.
  (I couldn't get the debug version to work - something I'm missing there.)


### You should be able to run the project now - there's quite a bit that the code does - so here's a breakdown of current functionality: -


When the app starts - it should flash the coloured LED's at the bottom of the bot.
Then - you can use the infrared remote that comes with the bot to drive it around a bit.

Also - if you have managed to wire up the distance sensor - the bot will beep and ignore your "forward" commands if there is an obstacle detected in front of the bot.

Finally - if you put something close to the proximity sensor - then the bot will turn the camera towards the side where the sensor detects something (if both sides detect something - then the camera is returned to the middle position).

### Some notes about the code pattern
So - the basic concept is that we have an "alphabot" class that does all the interacting with the hardware.

Then there are different services that monitor the different sensors like an infrared service that monitors the IR sensor and raises an event whenever it receives and IR signal.

All this hardware interface is controlled by a "behaviour" class.
You can use this class to get events when the hardware picks up signals, and to decide how the bot should behave.

If you look in the **main.cpp** file
- you will see how the alphabot class is initialised,
- and that all its events are captured and passed to any behaviors defined in that file.

To start fiddling with the code - you can look at the **sample_test_behaviour.cpp** file.
Here is all the code that drives the bot's behaviour described in the **breakdown of current functionalty**.

