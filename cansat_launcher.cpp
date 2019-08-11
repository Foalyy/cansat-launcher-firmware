#include <core.h>
#include <scif.h>
#include <pm.h>
#include <gpio.h>
#include <i2c.h>
#include <spi.h>
#include <adc.h>
#include "peripherals.h"
#include "drivers/leds.h"
#include "drivers/servo.h"
#include "drivers/battery.h"
#include "drivers/altimeter.h"
#include "drivers/telem.h"

void error();

int main() {
    Core::init();
    SCIF::enableRCFAST(SCIF::RCFASTFrequency::RCFAST_12MHZ);
    PM::setMainClockSource(PM::MainClockSource::RCFAST);

    // Init LEDs
    LEDs::init();

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

    // Init config switches
    GPIO::enableInput(PIN_CFG_1, GPIO::Pulling::PULLUP);
    GPIO::enableInput(PIN_CFG_2, GPIO::Pulling::PULLUP);
    GPIO::enableInput(PIN_CFG_3, GPIO::Pulling::PULLUP);
    GPIO::enableInput(PIN_CFG_4, GPIO::Pulling::PULLUP);

    // Init battery measurement
    ADC::init();
    Battery::init();

    // Init servo
    Servo::init();
    Servo::close();

    // Init altimeter
    if (!Altimeter::init()) {
        error();
    }

    // Init telem
    if (!Telem::init()) {
        error();
    }

    // Current state
    const unsigned long DELAY_BLINK_HATCH_STATUS = 1000;
    Core::Time lastTimeHatchStatusBlink = 0;

    // Main loop
    Core::Time t = Core::time();
    while (1) {
        // Read the launcher ID from the config switches
        int launcherID = (!GPIO::get(PIN_CFG_1)) | (!GPIO::get(PIN_CFG_2)) << 1 | (!GPIO::get(PIN_CFG_3)) << 2 | (!GPIO::get(PIN_CFG_4)) << 3;

        // Telem
        Telem::Command command = Telem::getCommand(launcherID);
        if (command == Telem::Command::OPEN) {
            Telem::sendAck(launcherID, command);
            Servo::open();
            LEDs::blink(LEDs::OPEN, 2);
            LEDs::blink(LEDs::TELEM);

        } else if (command == Telem::Command::CLOSE) {
            Telem::sendAck(launcherID, command);
            Servo::close();
            LEDs::blink(LEDs::CLOSE, 2);
            LEDs::blink(LEDs::TELEM);

        } else if (command == Telem::Command::GET_STATUS) {
            // Send the hatch status, the altitude and the temperature
            Telem::sendStatus(launcherID, Servo::isOpen(), Altimeter::altitude(), Altimeter::temperature(), Battery::percent());
            LEDs::blink(LEDs::TELEM);
        }

        // Buttons
        if (GPIO::fallingEdge(PIN_BTN_OPEN)) {
            Servo::open();
            LEDs::blink(LEDs::OPEN, 2);
        } else if (GPIO::fallingEdge(PIN_BTN_CLOSE)) {
            Servo::close();
            LEDs::blink(LEDs::CLOSE, 2);
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

        // Exec drivers
        LEDs::exec();
        Altimeter::exec();
        Battery::exec();

        // Sleep between two loop passes
        Core::sleep(1);
    }
}


void error() {
    const int DELAY = 300;
    Core::Time lastTimeBlinked = 0;
    while (true) {
        Core::Time t = Core::time();
        if (t > lastTimeBlinked + DELAY) {
            LEDs::blink(LEDs::CLOSE);
            lastTimeBlinked = t;
        }
        LEDs::exec();
        Core::sleep(1);
    }
}