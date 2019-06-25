#include <core.h>
#include <scif.h>
#include <pm.h>
#include <gpio.h>
#include <i2c.h>
#include <spi.h>
#include "peripherals.h"
#include "drivers/leds.h"
#include "drivers/servo.h"
#include "drivers/altimeter.h"

int main() {
    Core::init();
    SCIF::enableRCFAST(SCIF::RCFASTFrequency::RCFAST_12MHZ);
    PM::setMainClockSource(PM::MainClockSource::RCFAST);

    // Init LEDs
    LEDs::init();
    LEDs::on(LEDs::POWER);

    // Init SPI
    SPI::setPin(SPI::PinFunction::MISO, PIN_SPI_MISO);
    SPI::setPin(SPI::PinFunction::MOSI, PIN_SPI_MOSI);
    SPI::setPin(SPI::PinFunction::SCK, PIN_SPI_SCK);
    SPI::enableMaster();

    // Init I2C
    I2C::setPin(I2C_PORT, I2C::PinFunction::SDA, PIN_I2C_SDA);
    I2C::setPin(I2C_PORT, I2C::PinFunction::SCL, PIN_I2C_SCL);
    I2C::enableMaster(I2C_PORT);

    // Init buttons
    GPIO::enableInput(PIN_BTN_OPEN, GPIO::Pulling::PULLUP);
    GPIO::enableInput(PIN_BTN_CLOSE, GPIO::Pulling::PULLUP);

    // Init servo
    Servo::init();
    Servo::close();

    // Init altimeter
    Altimeter::init();

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
        } else if (GPIO::fallingEdge(PIN_BTN_CLOSE)) {
            Servo::close();
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

        // Exec drivers
        LEDs::exec();
        Altimeter::exec();

        // Sleep between two loop passes
        Core::sleep(1);
    }
}