#include <core.h>
#include <scif.h>
#include <pm.h>
#include "drivers/leds.h"

int main() {
    Core::init();
    SCIF::enableRCFAST(SCIF::RCFASTFrequency::RCFAST_12MHZ);
    PM::setMainClockSource(PM::MainClockSource::RCFAST);

    // Init LEDs
    LEDs::init();

    // ...

    // Initialization finished, start animating the power led
    LEDs::animate(LEDs::POWER);

    while (1) {

        // Exec LEDs driver
        LEDs::exec();

        // Sleep between two loop passes
        Core::sleep(1);
    }
}