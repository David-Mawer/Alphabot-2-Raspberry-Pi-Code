#include "led_service.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <exception>

led_service::led_service() : ledAccess()
{
    //ctor
}

void led_service::initialise() {

    ledstring.channel[0].invert = 0;
    ledstring.channel[0].strip_type = STRIP_TYPE;
    ledstring.channel[0].brightness = 200;
    ledstring.channel[1].invert = 0;

    ws2811_return_t initRes;
    if ((initRes = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(initRes));
        return;
    }

    led_strip_set_all(0);

    botInitialised = true;
}

void led_service::turnOff(int ledNo) {
    ledAccess.lock();
	if ((ledNo < 0) || (ledNo > 3)) {
        led_strip_set_all(0);
	} else {
        led_strip[ledNo] = 0;
	}
	led_strip_render();
    ledAccess.unlock();
}
void led_service::setBrightness(int nBrightness){
    ledAccess.lock();
    ledstring.channel[0].brightness = nBrightness;
    ws2811_render(&ledstring);
    ledAccess.unlock();
}
void led_service::setColour(int ledNo, uint32_t colour){
    ledAccess.lock();
	if ((ledNo < 0) || (ledNo > 3)) {
        led_strip_set_all(colour);
	} else {
        led_strip[ledNo] = colour;
	}
	led_strip_render();
    ledAccess.unlock();
}
void led_service::setAllColour(uint32_t colour0, uint32_t colour1, uint32_t colour2, uint32_t colour3){
    ledAccess.lock();
    led_strip[0] = colour0;
    led_strip[1] = colour1;
    led_strip[2] = colour2;
    led_strip[3] = colour3;
	led_strip_render();
    ledAccess.unlock();
}
void led_service::setSameOnAll(uint32_t colour) {
    ledAccess.lock();
    led_strip_set_all(colour);
	led_strip_render();
    ledAccess.unlock();
}



void led_service::finalise() {
    botInitialised = false;

    // turn of the LEDs
	turnOff(-1);

    // disconnect from the LED strip
    ws2811_fini(&ledstring);
}

ws2811_return_t led_service::led_strip_render(void)
{
    int x;

    for (x = 0; x < led_count; x++)
    {
        ledstring.channel[0].leds[x] = led_strip[x];
    }
    return ws2811_render(&ledstring);
}

void led_service::led_strip_set_all(uint32_t colour)
{
    int x;

    for (x = 0; x < led_count; x++)
    {
        led_strip[x] = colour;
    }
}
