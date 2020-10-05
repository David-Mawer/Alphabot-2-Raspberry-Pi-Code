#include <thread>     // for threading out loud
#include <functional>
#include <list>
#include <iostream>
#include "thread_helper.h"

thread_helper::thread_helper() : threadList()
{
    //ctor
}

void thread_helper::joinThreads() {
    // join all the threads to this main one (required to finish off cleanly).
    for (std::thread &myThread : threadList)
    {
        if (myThread.joinable()) {
            myThread.join();
        }
    }
}

void thread_helper::startThread(std::function<void()> functionToStart) {
    threadList.push_back(std::thread(functionToStart));
}
