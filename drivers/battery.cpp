#include "battery.h"
#include "../peripherals.h"
#include <core.h>
#include <adc.h>

namespace Battery {

    const int DELAY_BETWEEN_MEASURES = 1000;
    const int DELAY_COMMAND = 100;
    Core::Time _tMeasure = 0;
    float _voltage = 0;
    int _percent = 0;
    bool _switchOn = false;

    void init() {
        // Enable the command pin
        GPIO::enableOutput(PIN_VBAT_MEAS_CMD, GPIO::LOW);

        // Enable the ADC
        ADC::enable(ADC_VBAT_MEAS);
    }

    void exec() {
        Core::Time t = Core::time();
        if (t >= _tMeasure + DELAY_BETWEEN_MEASURES) {
            if (t < _tMeasure + DELAY_BETWEEN_MEASURES + DELAY_COMMAND) {
                // Then the switch on
                if (!_switchOn) {
                    GPIO::set(PIN_VBAT_MEAS_CMD, GPIO::HIGH);
                    _switchOn = true;
                }
            } else {
                // Measure a few values
                const int N_MEASURES_AVERAGE = 5;
                int sum = 0;
                for (int i = 0; i < N_MEASURES_AVERAGE; i++) {
                    sum += ADC::read(ADC_VBAT_MEAS);
                    Core::waitMicroseconds(1000);
                }

                // Turn the switch off
                GPIO::set(PIN_VBAT_MEAS_CMD, GPIO::LOW);
                _switchOn = false;

                // Compute the voltage and percentage
                int average = sum / N_MEASURES_AVERAGE;
                _voltage = 2 * average / 1000.;
                if (_voltage > 4.2) {
                    _percent = 100;
                } else if (_voltage < 3) {
                    _percent = 0;
                } else {
                    _percent = 100 * (_voltage - 3.4) / 0.8;
                }

                // Save the time of the current measurement
                _tMeasure = t;
            }
        }
    }

    float voltage() {
        return _voltage;
    }

    int percent() {
        return _percent;
    }

}