#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H
#include <functional>
#include <list>
#include <thread>     // for threading out loud

class thread_helper
{
    public:
        thread_helper();
        void joinThreads();
        void startThread(std::function<void()>);

    protected:

    private:
        std::list<std::thread> threadList;
};

#endif // THREAD_HELPER_H
