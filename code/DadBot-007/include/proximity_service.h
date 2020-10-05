#ifndef PROXIMITY_SERVICE_H
#define PROXIMITY_SERVICE_H
#include <atomic>
#include <functional>
#include <vector>


class proximity_service
{
    public:
        typedef std::function<void(const bool, const bool)> TProximityResultCallback;
        // typedef std::vector<TProximityResultCallback> TProximityResultCallbackVector;
        proximity_service(TProximityResultCallback);
        void run();

    protected:

    private:
        TProximityResultCallback m_result_callback = nullptr;
        // Data member holding callbacks (used to pass bluetooth events back to the caller.
        //TProximityResultCallbackVector m_result_callbacks;
        bool isProximity(std::string);
};

#endif // PROXIMITY_SERVICE_H
