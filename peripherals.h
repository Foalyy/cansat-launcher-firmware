#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include <gpio.h>
#include <tc.h>

const TC::Channel TC_LED_POWER = TC::TC0_0A;
const TC::Channel TC_LED_OPEN = TC::TC0_0B;
const TC::Channel TC_LED_CLOSE = TC::TC0_1A;
const GPIO::Pin PIN_LED_POWER = {GPIO::Port::A,  8, GPIO::Periph::B};
const GPIO::Pin PIN_LED_OPEN = {GPIO::Port::A,  9, GPIO::Periph::B};
const GPIO::Pin PIN_LED_CLOSE = {GPIO::Port::A, 10, GPIO::Periph::B};

#endif