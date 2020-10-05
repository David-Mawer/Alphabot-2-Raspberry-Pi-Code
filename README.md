
# Alphabot-2-Raspberry-Pi-Code

This is a C++ program to manage an Alphabot 2 using a Raspberry Pi 4.

few months ago I got an AlphaBot 2 (the one that comes with the Raspberry Pi 4).

Some googling told me that it was a good idea to use C or C++ when writing code for the bot
(especially if I know that performance is going to be an issue later on in my coding).

There were very few examples in C or C++ (most samples were written in Python).

So I wrote a C++ program to run the AlphaBot 2.

Here's the result of my experimentation...
...It's a program written in C++ that handles all the bits of hardware that makes up an AlphaBot 2.

---

### Instructions to get the code compiled
  **This bit's important - don't skip it!**

1. Install libopencv-dev using the command   **sudo apt install libopencv-dev**
  
2. copy the external libs to your local lib folder by running   **./code/DadBot-007/copy-lib-local.sh**

3. load up the project file in **code/DadBot-007/DadBot-007.cbp** using Code::Blocks - and build the **Release** version.   (I couldn't get the debug version to work - something I'm missing there.)

---

**The code should now work.**

If it doesn't work - then you'll need to figure out which dependency hasn't been installed.

Now - when the project runs - there are a few features that will work.

---

**Here's a breakdown of current functionality: -**

When the app starts - it should flash the coloured LED's at the bottom of the bot.

You can also use the infrared remote that comes with the bot to drive it around a bit.

You may have managed to wire up the distance sensor

> This sensor is not supported by default for the Raspberry Pi version
> of the bot - but it's possible to widen the gap between the top
> circuit board and the bottom circuit board of the bot.
> 
> Then you can use the distance sensor.

If you've got the distance sensor installed - then the bot will beep and ignore your "forward" commands if there is an obstacle detected in front of the bot.

Finally - if you put something close to one of the the proximity sensors *(these are in front of the bot; a bit to the left, and a bit to the right of center)* - then the bot will turn the camera towards the side where the sensor detects something.

If both sides detect something - then the camera is returned to the middle position.

---

### Some notes about the code pattern

A very brief summary of the  basic concept is this: -

We have an **"alphabot"** class that does all the interacting with the hardware.
There are different services that monitor the different sensors 

> like an infrared service that monitors the IR sensor and raises an
> event whenever it receives and IR signal

The **alphabot** class manages all the services, and raises *events* when something significant has happened (a service reports an event).

These *events* are captured in **main.cpp**.

Here the *events* are passed to a **"behaviour"** class.

The idea is that we only have to write code in the **behaviour** class.

The rest of the program is structured so that a **behaviour** can control the bot's actions - and it also has access to all the bot's sensors.

There's an example **sample_test_behaviour.cpp** class already in the project.

This **behaviour** is what makes the bot act as described in the **breakdown of current functionalty** section (above).

---

### **Note on many behaviours:**

It's possible to have more than one **behaviour** class.
One must think carefully to prevent the different **behaviour** classes from sending conflicting instructions to the **alphabot** class.

> e.g. You can have one **behaviour** to flash the coloured LED's
> and a different **behaviour** to drive the bot.
> These two will not conflict with each other.
