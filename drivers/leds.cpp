#include "leds.h"
#include "../peripherals.h"
#include <core.h>
#include <tc.h>
#include <string.h>

namespace LEDs {

    const int N_LEDS = 3;
    struct LedData {
        TC::Channel tc;
        GPIO::Pin pin;
        int gain;
        bool blink;
        bool blinkState;
        unsigned int blinkTimes;
        unsigned long blinktStart;
        unsigned long blinkDelay;
        bool animate;

    };
    LedData _leds[N_LEDS];

    int _animationCurrentLed = 0;
    int _animationCurrentPower = 0;
    bool _animationCurrentReverse = false;
    Core::Time _lastTimeAnimation = 0;
    const int ANIMATION_DELAY = 6;
    const int TC_PERIOD = 1000;


    void init() {
        memset(_leds, 0, sizeof(_leds));
        _leds[TELEM] = {
            .tc = TC_LED_TELEM,
            .pin = PIN_LED_TELEM,
            .gain = 3
        };
        _leds[OPEN] = {
            .tc = TC_LED_OPEN,
            .pin = PIN_LED_OPEN,
            .gain = 7
        };
        _leds[CLOSE] = {
            .tc = TC_LED_CLOSE,
            .pin = PIN_LED_CLOSE,
            .gain = 7
        };

        for (int i = 0; i < N_LEDS; i++) {
            TC::setPin(_leds[i].tc, TC::PinFunction::OUT, _leds[i].pin);
            TC::enablePWM(_leds[i].tc, TC_PERIOD, TC_PERIOD, true);
        }
    }

    // Internal
    void set(int led, int power) {
        if (power < 0) {
            power = 0;
        } else if (power > 100) {
            power = 100;
        }
        TC::setHighTime(_leds[led].tc, TC_PERIOD - power * _leds[led].gain);
    }

    void on(int led) {
        set(led, 100);
    }

    void off(int led) {
        set(led, 0);
    }

    void blink(int led, int times, unsigned long delay) {
        _leds[led].blink = true;
        _leds[led].blinkState = true;
        _leds[led].blinkTimes = times;
        _leds[led].blinktStart = Core::time();
        _leds[led].blinkDelay = delay;
        on(led);
    }

    void exec() {
        unsigned long t = Core::time();
        bool animating = false;

        for (int led = 0; led < N_LEDS; led++) {
            if (_leds[led].animate) {
                animating = true;
            }

            if (_leds[led].blink) {
                unsigned long p = (t - _leds[led].blinktStart) / _leds[led].blinkDelay;
                if (p >= 2 * _leds[led].blinkTimes - 1) {
                    // Finished
                    off(led);
                    _leds[led].blink = false;
                }
                if (p % 2 == 0 && _leds[led].blinkState == false) {
                    on(led);
                    _leds[led].blinkState = true;
                } else if (p % 2 == 1 && _leds[led].blinkState == true) {
                    off(led);
                    _leds[led].blinkState = false;
                }
            }
        }

        if (animating && t > _lastTimeAnimation + ANIMATION_DELAY && !_leds[_animationCurrentLed].blink) {
            set(_animationCurrentLed, _animationCurrentPower);
            if (!_animationCurrentReverse) {
                _animationCurrentPower++;
                if (_animationCurrentPower > 100) {
                    _animationCurrentPower = 100;
                    _animationCurrentReverse = true;
                }
            } else {
                _animationCurrentPower--;
                if (_animationCurrentPower < 0) {
                    _animationCurrentPower = 0;
                    _animationCurrentReverse = false;
                    for (int led = 0; led < N_LEDS; led++) {
                        off(led);
                    }
                    for (int i = 0; i < N_LEDS; i++) {
                        _animationCurrentLed++;
                        if (_animationCurrentLed > N_LEDS - 1) {
                            _animationCurrentLed = 0;
                        }
                        if (_leds[_animationCurrentLed].animate) {
                            break;
                        }
                    }
                }
            }

            _lastTimeAnimation = t;
        }
    }

    void setLedGain(int led, int gain) {
        _leds[led].gain = gain;
    }

    void animate(int led, bool enabled) {
        bool alreadyAnimating = false;
        for (int led = 0; led < N_LEDS; led++) {
            if (_leds[led].animate) {
                alreadyAnimating = true;
                break;
            }
        }
        if (!alreadyAnimating) {
            _animationCurrentLed = led;
            _animationCurrentPower = 0;
            _animationCurrentReverse = false;
        }
        _leds[led].animate = enabled;
        if (!enabled) {
            off(led);
        }
    }

}