#ifndef BT_HANDLER_H
#define BT_HANDLER_H
#include <atomic>
#include <functional>

class bt_handler
{
    public:
        typedef std::function<void(std::string)> TBtResultCallback;
        bt_handler(TBtResultCallback);
        void run();

    protected:

    private:
        TBtResultCallback m_result_callback = nullptr;
        std::string convertToString(char* a);
};

#endif // BT_HANDLER_H
