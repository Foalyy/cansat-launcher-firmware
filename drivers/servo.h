#ifndef _SERVO_H_
#define _SERVO_H_

namespace Servo {

    static const unsigned int PERIOD = 10000;
    static const unsigned int HIGH_TIME_OPEN = 1300;
    static const unsigned int HIGH_TIME_CLOSE = 1800;

    void init();
    void open();
    void close();
    bool isOpen();

}

#endif