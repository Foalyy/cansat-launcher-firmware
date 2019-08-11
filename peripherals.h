#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include <gpio.h>
#include <tc.h>
#include <i2c.h>
#include <spi.h>

// I2C port
const I2C::Port I2C_PORT = I2C::Port::I2C1;
const GPIO::Pin PIN_I2C_SDA = {GPIO::Port::B,  0, GPIO::Periph::A};
const GPIO::Pin PIN_I2C_SCL = {GPIO::Port::B,  1, GPIO::Periph::A};

// SPI port
const GPIO::Pin PIN_SPI_MISO = {GPIO::Port::A, 27, GPIO::Periph::A};
const GPIO::Pin PIN_SPI_MOSI = {GPIO::Port::A, 28, GPIO::Periph::A};
const GPIO::Pin PIN_SPI_SCK = {GPIO::Port::A, 29, GPIO::Periph::A};

// LEDs
const TC::Channel TC_LED_TELEM = TC::TC0_0A;
const TC::Channel TC_LED_OPEN = TC::TC0_0B;
const TC::Channel TC_LED_CLOSE = TC::TC0_1A;
const GPIO::Pin PIN_LED_TELEM = {GPIO::Port::A,  8, GPIO::Periph::B};
const GPIO::Pin PIN_LED_OPEN = {GPIO::Port::A,  9, GPIO::Periph::B};
const GPIO::Pin PIN_LED_CLOSE = {GPIO::Port::A, 10, GPIO::Periph::B};

// Buttons
const GPIO::Pin PIN_BTN_OPEN = GPIO::PB04;
const GPIO::Pin PIN_BTN_CLOSE = GPIO::PB05;

// Config switches
const GPIO::Pin PIN_CFG_1 = GPIO::PA05;
const GPIO::Pin PIN_CFG_2 = GPIO::PA04;
const GPIO::Pin PIN_CFG_3 = GPIO::PB03;
const GPIO::Pin PIN_CFG_4 = GPIO::PB02;

// LoRa
const SPI::Peripheral SPI_SLAVE_LORA = 0;
const GPIO::Pin PIN_LORA_DIO0 = GPIO::PA11;
const GPIO::Pin PIN_LORA_DIO1 = GPIO::PA12;
const GPIO::Pin PIN_LORA_DIO2 = GPIO::PA31;
const GPIO::Pin PIN_LORA_DIO3 = GPIO::PB06;
const GPIO::Pin PIN_LORA_DIO4 = GPIO::PB07;
const GPIO::Pin PIN_LORA_DIO5 = GPIO::PA06;
const GPIO::Pin PIN_LORA_RESET = GPIO::PA13;
const GPIO::Pin PIN_LORA_CS = {GPIO::Port::A, 30, GPIO::Periph::A};

// Servo
const TC::Channel TC_SERVO = TC::TC0_2B;
const GPIO::Pin PIN_SERVO = {GPIO::Port::B, 12, GPIO::Periph::D};

// Altimeter
const GPIO::Pin PIN_ALTIMETER_INT = GPIO::PA14;

// AUX connector
const GPIO::Pin PIN_AUX_GPIO1 = GPIO::PA18;
const GPIO::Pin PIN_AUX_GPIO2 = GPIO::PA19;

// Battery voltage measurement
const GPIO::Pin PIN_VBAT_MEAS_CMD = GPIO::PB11;
const GPIO::Pin PIN_VBAT_MEAS = {GPIO::Port::A,  7, GPIO::Periph::A};
const int ADC_VBAT_MEAS = 2;


#endif