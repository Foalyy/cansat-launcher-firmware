#ifndef _LED_H_
#define _LED_H_

#include <tc.h>

namespace LEDs {

    const int TELEM = 0;
    const int OPEN = 1;
    const int CLOSE = 2;

    void init();
    void set(int led, int power);
    void setColor(int led, int r, int g, int b);
    void on(int led);
    void off(int led);
    void blink(int, int times=1, unsigned long delay=100);
    void exec();
    void setLedGain(int led, int gain=10);
    void animate(int led, bool enabled=true);

}

#endif
