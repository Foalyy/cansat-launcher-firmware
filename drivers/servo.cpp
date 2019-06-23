#include "servo.h"
#include "../peripherals.h"

namespace Servo {

    bool _open = false;

    void init() {
        TC::setPin(TC_SERVO, TC::PinFunction::OUT, PIN_SERVO);
        TC::enablePWM(TC_SERVO, PERIOD, HIGH_TIME_CLOSE, true);
    }

    void open() {
        TC::setHighTime(TC_SERVO, HIGH_TIME_OPEN);
        _open = true;
    }

    void close() {
        TC::setHighTime(TC_SERVO, HIGH_TIME_CLOSE);
        _open = false;
    }

    bool isOpen() {
        return _open;
    }

}