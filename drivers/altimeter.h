#ifndef _ALTIMETER_H_
#define _ALTIMETER_H_

#include <stdint.h>

namespace Altimeter {

    // I2C address with SDO pulled to VCC
    const uint8_t I2C_ADDR = 0b1110111;

    // Registers
    const uint8_t REG_CHIP_ID = 0x00;
    const uint8_t REG_ERR_REG = 0x02;
    const uint8_t REG_STATUS = 0x03;
    const uint8_t REG_DATA_0 = 0x04;
    const uint8_t REG_DATA_1 = 0x05;
    const uint8_t REG_DATA_2 = 0x06;
    const uint8_t REG_DATA_3 = 0x07;
    const uint8_t REG_DATA_4 = 0x08;
    const uint8_t REG_DATA_5 = 0x09;
    const uint8_t REG_SENSORTIME_0 = 0x0C;
    const uint8_t REG_SENSORTIME_1 = 0x0D;
    const uint8_t REG_SENSORTIME_2 = 0x0E;
    const uint8_t REG_SENSORTIME_3 = 0x0F;
    const uint8_t REG_EVENT = 0x10;
    const uint8_t REG_INT_STATUS = 0x11;
    const uint8_t REG_FIFO_LENGTH_0 = 0x12;
    const uint8_t REG_FIFO_LENGTH_1 = 0x13;
    const uint8_t REG_FIFO_DATA = 0x14;
    const uint8_t REG_FIFO_WTM_0 = 0x15;
    const uint8_t REG_FIFO_WTM_1 = 0x16;
    const uint8_t REG_FIFO_CONFIG_1 = 0x17;
    const uint8_t REG_FIFO_CONFIG_2 = 0x18;
    const uint8_t REG_INT_CTRL = 0x19;
    const uint8_t REG_IF_CONF = 0x1A;
    const uint8_t REG_PWR_CTRL = 0x1B;
    const uint8_t REG_OSR = 0x1C;
    const uint8_t REG_ODR = 0x1D;
    const uint8_t REG_CONFIG = 0x1F;
    const uint8_t REG_NVM_PAR_T1 = 0x31;
    const uint8_t REG_CMD = 0x7E;

    // Subregisters
    const uint8_t REG_STATUS_DRDY_PRESS = 5; // Data ready for pressure
    const uint8_t REG_STATUS_DRDY_TEMP = 6; // Data ready for temperature

    // Calibration coefficients
    struct CalibCoefsRaw {
        uint16_t nvm_par_t1;
        uint16_t nvm_par_t2;
        int8_t nvm_par_t3;
        int16_t nvm_par_p1;
        int16_t nvm_par_p2;
        int8_t nvm_par_p3;
        int8_t nvm_par_p4;
        uint16_t nvm_par_p5;
        uint16_t nvm_par_p6;
        int8_t nvm_par_p7;
        int8_t nvm_par_p8;
        int16_t nvm_par_p9;
        int8_t nvm_par_p10;
        int8_t nvm_par_p11;
    };
    struct CalibCoefs {
        float par_t1;
        float par_t2;
        float par_t3;
        float par_p1;
        float par_p2;
        float par_p3;
        float par_p4;
        float par_p5;
        float par_p6;
        float par_p7;
        float par_p8;
        float par_p9;
        float par_p10;
        float par_p11;
        float t_lin;
    };

    // Module functions
    bool init();
    void exec();
    float pressure();
    float altitude();
    float temperature();
    void readData();

}


#endif