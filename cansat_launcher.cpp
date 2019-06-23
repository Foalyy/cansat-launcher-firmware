#include <core.h>
#include <scif.h>
#include <pm.h>
#include <gpio.h>
#include "peripherals.h"
#include "drivers/leds.h"
#include "drivers/servo.h"

int main() {
    Core::init();
    SCIF::enableRCFAST(SCIF::RCFASTFrequency::RCFAST_12MHZ);
    PM::setMainClockSource(PM::MainClockSource::RCFAST);

    // Init LEDs
    LEDs::init();
    LEDs::on(LEDs::POWER);

    // Init buttons
    GPIO::enableInput(PIN_BTN_OPEN, GPIO::Pulling::PULLUP);
    GPIO::enableInput(PIN_BTN_CLOSE, GPIO::Pulling::PULLUP);

    // Init servo
    Servo::init();
    Servo::close();

    // Initialization finished, start blinking the power led
    LEDs::blink(LEDs::POWER);

    // Current state
    const unsigned long DELAY_BLINK_POWER = 1000;
    Core::Time lastTimePowerBlink = 500;
    const unsigned long DELAY_BLINK_HATCH_STATUS = 1000;
    Core::Time lastTimeHatchStatusBlink = 0;

    // Main loop
    Core::Time t = Core::time();
    while (1) {
        // Buttons
        if (GPIO::fallingEdge(PIN_BTN_OPEN)) {
            Servo::open();
            lastTimeHatchStatusBlink = 0;
        } else if (GPIO::fallingEdge(PIN_BTN_CLOSE)) {
            Servo::close();
            lastTimeHatchStatusBlink = 0;
        }

        // Hatch status leds blink
        t = Core::time();
        if (t >= lastTimeHatchStatusBlink + DELAY_BLINK_HATCH_STATUS) {
            if (Servo::isOpen()) {
                LEDs::blink(LEDs::OPEN);
            } else {
                LEDs::blink(LEDs::CLOSE);
            }
            lastTimeHatchStatusBlink += DELAY_BLINK_HATCH_STATUS;
        }

        // Power led blink
        t = Core::time();
        if (t >= lastTimePowerBlink + DELAY_BLINK_POWER) {
            LEDs::blink(LEDs::POWER);
            lastTimePowerBlink += DELAY_BLINK_POWER;
        }

        // Exec LEDs driver
        LEDs::exec();

        // Sleep between two loop passes
        Core::sleep(1);
    }
}