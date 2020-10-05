#ifndef IR_REMOTE_SERVICE_H
#define IR_REMOTE_SERVICE_H
#include <atomic>
#include <functional>

class ir_remote_service
{
    public:
        typedef std::function<void(int)> TIrResultCallback;
        ir_remote_service(TIrResultCallback);
        void run();

    protected:

    private:
        TIrResultCallback m_result_callback = nullptr;
        int lastKeyPress = -99;
        int getKey();
};

#endif // IR_REMOTE_SERVICE_H
