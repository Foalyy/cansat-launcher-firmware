#include "altimeter.h"
#include <string.h>
#include <math.h>
#include <core.h>
#include <i2c.h>
#include "../peripherals.h"

namespace Altimeter {

    bool _error = false;
    uint32_t _pressureRaw = 0;
    float _pressure = 0;
    float _pressure0 = 0;
    float _altitude = 0;
    uint32_t _temperatureRaw = 0;
    float _temperature = 0;
    CalibCoefsRaw _calibCoefsRaw;
    CalibCoefs _calibCoefs;
    Core::Time _lastTimeMeasured = 0;
    const unsigned long DELAY_MEASURE = 1000;
    Core::Time _lastTimeCheckedStatus = 0;
    const unsigned long DELAY_CHECK_STATUS = 100;


    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    

    bool init() {
        uint8_t chipId = readRegister(REG_CHIP_ID);
        if (chipId != 0x50) {
            _error = true;
            return false;
        }

        // Enable oversampling
        writeRegister(REG_OSR, 0b00000011);

        // Maximum data rate
        writeRegister(REG_ODR, 0x00);

        // Disable the IIR filter
        writeRegister(REG_CONFIG, 0);

        // Read calibration coefficients
        const int size = 21;
        uint8_t rxBuffer[size];
        I2C::writeRead(I2C_PORT, I2C_ADDR, REG_NVM_PAR_T1, rxBuffer, size);
        _calibCoefsRaw.nvm_par_t1 = rxBuffer[0] | rxBuffer[1] << 8;
        _calibCoefsRaw.nvm_par_t2 = rxBuffer[2] | rxBuffer[3] << 8;
        _calibCoefsRaw.nvm_par_t3 = rxBuffer[4];
        _calibCoefsRaw.nvm_par_p1 = rxBuffer[5] | rxBuffer[6] << 8;
        _calibCoefsRaw.nvm_par_p2 = rxBuffer[7] | rxBuffer[8] << 8;
        _calibCoefsRaw.nvm_par_p3 = rxBuffer[9];
        _calibCoefsRaw.nvm_par_p4 = rxBuffer[10];
        _calibCoefsRaw.nvm_par_p5 = rxBuffer[11] | rxBuffer[12] << 8;
        _calibCoefsRaw.nvm_par_p6 = rxBuffer[13] | rxBuffer[14] << 8;
        _calibCoefsRaw.nvm_par_p7 = rxBuffer[15];
        _calibCoefsRaw.nvm_par_p8 = rxBuffer[16];
        _calibCoefsRaw.nvm_par_p9 = rxBuffer[17] | rxBuffer[18] << 8;
        _calibCoefsRaw.nvm_par_p10 = rxBuffer[19];
        _calibCoefsRaw.nvm_par_p11 = rxBuffer[20];
        _calibCoefs.par_t1 = (float)(_calibCoefsRaw.nvm_par_t1) * (1 << 8);
        _calibCoefs.par_t2 = (float)(_calibCoefsRaw.nvm_par_t2) / (1 << 30);
        _calibCoefs.par_t3 = (float)(_calibCoefsRaw.nvm_par_t3) / ((uint64_t)1 << 48);
        _calibCoefs.par_p1 = (float)((_calibCoefsRaw.nvm_par_p1) - (1 << 14)) / (1 << 20);
        _calibCoefs.par_p2 = (float)((_calibCoefsRaw.nvm_par_p2) - (1 << 14)) / (1 << 29);
        _calibCoefs.par_p3 = (float)(_calibCoefsRaw.nvm_par_p3) / ((uint64_t)1 << 32);
        _calibCoefs.par_p4 = (float)(_calibCoefsRaw.nvm_par_p4) / ((uint64_t)1 << 37);
        _calibCoefs.par_p5 = (float)(_calibCoefsRaw.nvm_par_p5) * (1 << 3);
        _calibCoefs.par_p6 = (float)(_calibCoefsRaw.nvm_par_p6) / (1 << 6);
        _calibCoefs.par_p7 = (float)(_calibCoefsRaw.nvm_par_p7) / (1 << 8);
        _calibCoefs.par_p8 = (float)(_calibCoefsRaw.nvm_par_p8) / (1 << 15);
        _calibCoefs.par_p9 = (float)(_calibCoefsRaw.nvm_par_p9) / ((uint64_t)1 << 48);
        _calibCoefs.par_p10 = (float)(_calibCoefsRaw.nvm_par_p10) / ((uint64_t)1 << 48);
        _calibCoefs.par_p11 = (float)(_calibCoefsRaw.nvm_par_p11) / (4 * (float)((uint64_t)1 << 63));

        // Enable Forced mode with pressure and temperature sensors
        writeRegister(REG_PWR_CTRL, 0b00100011);

        return true;
    }

    void exec() {
        Core::Time t = Core::time();

        // Send a forced measurement command everytime the delay expires
        if (t >= _lastTimeMeasured + DELAY_MEASURE) {
            writeRegister(REG_PWR_CTRL, 0b00100011);
            _lastTimeMeasured = t;
        }

        // If some data is ready, read it and compute the pressure, altitude and temperature
        if (t >= _lastTimeCheckedStatus + DELAY_CHECK_STATUS) {
            uint8_t status = readRegister(REG_STATUS);
            if (status & (1 << REG_STATUS_DRDY_PRESS) && status & (1 << REG_STATUS_DRDY_TEMP)) {
                readData();
            }
            _lastTimeCheckedStatus = t;
        }
    }

    float pressure() {
        return _pressure;
    }

    float altitude() {
        return _altitude;
    }

    float temperature() {
        return _temperature;
    }

    void readData() {
        // Read the raw data from the sensor
        uint8_t rxBuffer[6];
        I2C::writeRead(I2C_PORT, I2C_ADDR, REG_DATA_0, rxBuffer, 6);
        _pressureRaw = rxBuffer[0] | rxBuffer[1] << 8 | rxBuffer[2] << 16;
        _temperatureRaw = rxBuffer[3] | rxBuffer[4] << 8 | rxBuffer[5] << 16;

        // Calculate compensated temperature
        float partial_data_t1 = (float)(_temperatureRaw - _calibCoefs.par_t1);
        float partial_data_t2 = (float)(partial_data_t1 * _calibCoefs.par_t2);
        _calibCoefs.t_lin = partial_data_t2 + (partial_data_t1 * partial_data_t1) * _calibCoefs.par_t3;
        _temperature = _calibCoefs.t_lin;

        // Calculate compensated pressure
        float partial_data_p1 = _calibCoefs.par_p6 * _calibCoefs.t_lin;
        float partial_data_p2 = _calibCoefs.par_p7 * (_calibCoefs.t_lin * _calibCoefs.t_lin);
        float partial_data_p3 = _calibCoefs.par_p8 * (_calibCoefs.t_lin * _calibCoefs.t_lin * _calibCoefs.t_lin);
        float partial_out_p1 = _calibCoefs.par_p5 + partial_data_p1 + partial_data_p2 + partial_data_p3;
        partial_data_p1 = _calibCoefs.par_p2 * _calibCoefs.t_lin;
        partial_data_p2 = _calibCoefs.par_p3 * (_calibCoefs.t_lin * _calibCoefs.t_lin);
        partial_data_p3 = _calibCoefs.par_p4 * (_calibCoefs.t_lin * _calibCoefs.t_lin * _calibCoefs.t_lin);
        float partial_out_p2 = (float)(_pressureRaw) * (_calibCoefs.par_p1 + partial_data_p1 + partial_data_p2 + partial_data_p3);
        partial_data_p1 = (float)(_pressureRaw) * (float)(_pressureRaw);
        partial_data_p2 = _calibCoefs.par_p9 + _calibCoefs.par_p10 * _calibCoefs.t_lin;
        partial_data_p3 = partial_data_p1 * partial_data_p2;
        float partial_data_p4 = partial_data_p3 + ((float)(_pressureRaw) * (float)(_pressureRaw) * (float)(_pressureRaw)) * _calibCoefs.par_p11;
        _pressure = partial_out_p1 + partial_out_p2 + partial_data_p4;

        // When the first measurement is made, save it as the ground-level pressure
        if (_pressure0 == 0) {
            _pressure0 = _pressure;
        }

        // Calculate altitude
        _altitude = ((pow(_pressure0 / _pressure, 0.19022256) - 1) * (_temperature + 273.15)) / 0.0065;
    }

    uint8_t readRegister(uint8_t reg) {
        uint8_t rxBuffer[1] = {0x00};
        I2C::writeRead(I2C_PORT, I2C_ADDR, reg, rxBuffer, 1);
        return rxBuffer[0];
    }

    void writeRegister(uint8_t reg, uint8_t value) {
        uint8_t txBuffer[2] = {reg, value};
        I2C::write(I2C_PORT, I2C_ADDR, txBuffer, 2);
    }

}