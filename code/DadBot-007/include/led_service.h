#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <stdlib.h>
#include <mutex>
#include <../libinc/ws2811.h>

#define STRIP_TYPE     WS2811_STRIP_GRB
#define LED_COUNT      4
#define TARGET_FREQ    WS2811_TARGET_FREQ
#define GPIO_PIN       18
#define DMA            10

class led_service
{
    public:
        led_service();
        void initialise();
        void finalise();
        void turnOff(int);
        void setBrightness(int);
        void setColour(int ledNo, uint32_t colour);
        void setAllColour(uint32_t colour0, uint32_t colour1, uint32_t colour2, uint32_t colour3);
        void setSameOnAll(uint32_t);

    protected:

    private:
        // Variables to help with the LED Strip: BEGIN
        int led_count = LED_COUNT;

        ws2811_t ledstring =
        {
            .freq = TARGET_FREQ,
            .dmanum = DMA,
            .channel =
            {
                [0] =
                {
                    .gpionum = GPIO_PIN,
                    .count = LED_COUNT,
                },
                [1] =
                {
                    .gpionum = 0,
                    .count = 0,
                    .brightness = 0,
                },
            },
        };

        ws2811_led_t *led_strip = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * led_count);
        // Variables to help with the LED Strip: BEGIN

        ws2811_return_t led_strip_render(void);
        void led_strip_set_all(uint32_t);
        bool botInitialised = false;
        // mutexes to prevent issues
        std::mutex ledAccess;

};

#endif // LED_SERVICE_H
