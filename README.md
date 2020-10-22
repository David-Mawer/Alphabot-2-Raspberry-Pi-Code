
# Manage WaveShare Alphabot2-Pi with C++

This is a C++ program to manage an Alphabot 2 using a Raspberry Pi 4.

few months ago I got an AlphaBot 2 (the one that comes with the Raspberry Pi 4).

There were very few examples to manage the bot in C or C++ (most samples were written in Python).

So I learnt C++ to write a program to run the AlphaBot 2.

Here's the result of my experimentation...

...It's a program written in C++ that handles all the hardware components that make up an AlphaBot 2.

The big win for me is to have one class where the bot can be controlled.

This is explained below in the section **Some notes about the code pattern**.

---

### Some notes about the code pattern

The idea behind the pattern is to have only one place where we need to write code in order to control the behaviour of the bot.

Here's a quick outline of the main classes in the program - and how they are linked together to achieve this.

We have an **"alphabot"** class that is responsible for all the interacting with the hardware.

This class creates a thread to run a service for each one of the different sensors.

The service has a call-back (or an event) thast it uses to pass information on any events that it detects.

> like an infrared service that monitors the IR sensor and raises an
> event whenever it receives and IR signal

The idea was that the **alphabot** class would handle these *events*.

It turns out that C++ frowns on events being handled by a method in an instance of a class.

For this reason - the service's *events* are captured together with any other **aplhabot** events in **main.cpp**.

This is where all *events* are forwarded to a **"behaviour"** class.



So now we can write code in the **behaviour** class that controls the bot's movement, and responds to all the sensors.



There's an example **sample_test_behaviour.cpp** class already in the project.

This **behaviour** is what makes the bot act as described in the **breakdown of current functionality** section (below).



**Note on more than one behaviour:**

It's possible to have more than one **behaviour** class.
One must think carefully to prevent the different **behaviour** classes from sending conflicting instructions to the **alphabot** class.

> e.g. You can have one **behaviour** to flash the coloured LED's
> and a different **behaviour** to drive the bot.
> These two will not conflict with each other.

---

### Installing the code and getting it running
  **This bit's important - don't skip it!**

1. clone the master branch into it's own folder on the bot's Raspberry Pi.

2. Install libopencv-dev using the command   **sudo apt install libopencv-dev**
  
3. copy the external libs to your local lib folder by running   **./code/DadBot-007/copy-lib-local.sh**

4. load up the project file in **code/DadBot-007/DadBot-007.cbp** using Code::Blocks - and build the **Release** version.   (I couldn't get the debug version to work - something I'm missing there.)


**The code should work now.**

If it doesn't work - then you'll need to figure out which dependency hasn't been installed.

Code::Blocks will show decent compile-time error messages; you can google these to figure out which libraries should be installed.

I have assumed that you have the basic C++ toolset installed.

---

### Breakdown of current functionality: -

When the app starts - it should flash the coloured LED's at the bottom of the bot (for a short time).

You can also be able to use the infrared remote that comes with the bot to drive it around a bit
(be careful - the top middle button will turn off the bot).

If you put something close to one of the proximity sensors *(these are in front of the bot; a bit to the left, and a bit to the right of centre)* - then the bot will turn the camera towards the side where the sensor detects something.

If both sides detect something - then the camera is returned to the middle position.

Finally - if you've got the distance sensor installed - The bot will beep and ignore your "forward" commands if there is an obstacle detected too close to the front of the bot.

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

---

### ToDo:

(notes to myself on what should still happen before the code pattern is completely implemented as planned)

1. Move camera processing logic into it's own service (see YouTube video for sample of processing frames: https://www.youtube.com/watch?reload=9&v=tpwokAPiqfs&t=11s)
2. Consider adding line following to the default behaviour (maybe use a button on the I.R. remote to start / stop the line follow logic).
3. consider adding some image processing intelligence to the default behaviour.
