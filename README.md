
# Manage WaveShare Alphabot2-Pi with C++

This is a C++ program to manage an Alphabot 2 using a Raspberry Pi 4.

few months ago I got an AlphaBot 2 (the one that comes with the Raspberry Pi 4).

There were very few examples to manage the bot in C or C++ (most samples were written in Python).

So I learnt C++ to write a program to run the AlphaBot 2.

Here's the result of my experimentation...
...It's a program written in C++ that handles all the hardware components that make up an AlphaBot 2.

The big win for me is to have one class where the bot can be controlled. This is explained in "Some notes about the code pattern".

---

### Some notes about the code pattern

A very brief summary of the basic concept: -

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

This **behaviour** is what makes the bot act as described in the **breakdown of current functionality** section (above).

---

### **Note on more than one behaviour:**

It's possible to have more than one **behaviour** class.
One must think carefully to prevent the different **behaviour** classes from sending conflicting instructions to the **alphabot** class.

> e.g. You can have one **behaviour** to flash the coloured LED's
> and a different **behaviour** to drive the bot.
> These two will not conflict with each other.

---

**Here's a breakdown of current functionality: -**

When the app starts - it should flash the coloured LED's at the bottom of the bot (for a short time).

You can also be able to use the infrared remote that comes with the bot to drive it around a bit
(be careful - the top middle button will turn off the bot).

Now - the distance sensor does not come with the Pi version of the bot.
It's possible to buy this sensor separately

> Then, it's possible to widen the gap between the top
> circuit board and the bottom circuit board of the bot.
> 
> If you make the gap big enough - then you can 
> plug in the distance sensor (and it works).
> 
> Here's a picture of the modified AlphaBot 2 (Pi)
> You can see the black pieces that make the space for the sonar to be installed.
> 
> ![Image of DadBot-007](https://raw.githubusercontent.com/David-Mawer/Alphabot-2-Raspberry-Pi-Code/main/DadBot-007%20with%20Sonar.jpg)
> (the distance sonar is the blue piece that's got 2 cylinders attached to it)
> 
> If you're wandering what's happening at the back of the bot ... well - I added a battery holder so that I could power the bot with two 18650 batteries.
>
> This was necessary because the AlphaBot 2 comes with a battery holder that can only handle 14500 batteries (these are shorter than the 18650 ones).
> This was just something I did to improve the battery life.

If you've got the distance sensor installed - then the bot will beep and ignore your "forward" commands if there is an obstacle detected in front of the bot.

Finally (in the list of features) - if you put something close to one of the proximity sensors *(these are in front of the bot; a bit to the left, and a bit to the right of centre)* - then the bot will turn the camera towards the side where the sensor detects something.

If both sides detect something - then the camera is returned to the middle position.

---

### Instructions to get the code compiled
  **This bit's important - don't skip it!**

1. Install libopencv-dev using the command   **sudo apt install libopencv-dev**
  
2. copy the external libs to your local lib folder by running   **./code/DadBot-007/copy-lib-local.sh**

3. load up the project file in **code/DadBot-007/DadBot-007.cbp** using Code::Blocks - and build the **Release** version.   (I couldn't get the debug version to work - something I'm missing there.)

---

**The code should now work.**

If it doesn't work - then you'll need to figure out which dependency hasn't been installed.

Code::Blocks will show decent compile-time error messages; you can google these to figure out which libraries should be installed.

I have assumed that you have the basic C++ toolset installed.

So - when the project runs - then the rest of this README will make more sense to you.


---

### ToDo:

(notes to myself on what should still happen before the code pattern is completely implemented as planned)

1. Move camera processing logic into it's own service (see YouTube video for sample of processing frames: https://www.youtube.com/watch?reload=9&v=tpwokAPiqfs&t=11s)
2. Consider adding line following to the default behaviour (maybe use a button on the I.R. remote to start / stop the line follow logic).
3. consider adding some image processing intelligence to the default behaviour.
