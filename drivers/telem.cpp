#include "telem.h"
#include <spi.h>
#include "../peripherals.h"

namespace Telem {


    bool init() {
        LoRa::setPin(LoRa::PinFunction::RESET, PIN_LORA_RESET);
        SPI::setPin(static_cast<SPI::PinFunction>(static_cast<int>(SPI::PinFunction::CS0) + static_cast<int>(SPI_SLAVE_LORA)), PIN_LORA_CS);
        if (!LoRa::init(SPI_SLAVE_LORA, 869350000L)) {
            return false;
        }
        LoRa::setTxPower(10); // dBm
        LoRa::setSpreadingFactor(10);
        LoRa::setCodingRate(LoRa::CodingRate::RATE_4_8);
        LoRa::setBandwidth(LoRa::Bandwidth::BW_62_5kHz);
        LoRa::setExplicitHeader(true);
        LoRa::enableRx();
        return true;
    }

    Command getCommand(uint8_t launcherID) {
        // Check if some data has been received in the LoRa's FIFO
        if (LoRa::rxAvailable()) {
            // Retrieve this data
            const int BUFFER_RX_SIZE = 4;
            uint8_t rxBuffer[BUFFER_RX_SIZE];
            int rxSize = LoRa::rx(rxBuffer, BUFFER_RX_SIZE);
            // Check that this looks like a valid frame on the same channel
            if (rxSize == BUFFER_RX_SIZE 
                    && rxBuffer[0] == 'C' && rxBuffer[1] == 'S'
                    && rxBuffer[2] == launcherID
                    && (rxBuffer[3] == static_cast<uint8_t>(Command::OPEN)
                        || rxBuffer[3] == static_cast<uint8_t>(Command::CLOSE)
                        || rxBuffer[3] == static_cast<uint8_t>(Command::GET_STATUS))) {
                return static_cast<Command>(rxBuffer[3]);
            }
        }
        return Command::NONE;
    }

    void sendCommand(uint8_t launcherID, Command command) {
        const int SIZE = 4;
        uint8_t buffer[SIZE] = {
                'c', 's',
                static_cast<uint8_t>(launcherID),
                static_cast<uint8_t>(command)
        };
        LoRa::tx(buffer, SIZE);
    }

    void sendAck(uint8_t launcherID, Command command) {
        const int SIZE = 5;
        uint8_t buffer[SIZE] = {
                'c', 's',
                static_cast<uint8_t>(launcherID),
                static_cast<uint8_t>(Command::ACK),
                static_cast<uint8_t>(command)
        };
        LoRa::tx(buffer, SIZE);
    }

    void sendStatus(uint8_t launcherID, bool hatchOpen, float altitude, float temperature, int batteryPercent) {
        int altitudeInt = altitude * 10;
        if (altitudeInt < 0) {
            altitudeInt = 0;
        } else if (altitudeInt > 65535) {
            altitudeInt = 65535;
        }
        int temperatureInt = temperature * 10;
        if (temperatureInt < 0) {
            temperatureInt = 0;
        } else if (temperatureInt > 65535) {
            temperatureInt = 65535;
        }
        const int SIZE = 10;
        uint8_t buffer[SIZE] = {
                'c', 's',
                static_cast<uint8_t>(launcherID),
                static_cast<uint8_t>(Command::STATUS),
                static_cast<uint8_t>(hatchOpen),
                static_cast<uint8_t>((altitudeInt >> 8) & 0xFF),
                static_cast<uint8_t>(altitudeInt & 0xFF),
                static_cast<uint8_t>((temperatureInt >> 8) & 0xFF),
                static_cast<uint8_t>(temperatureInt & 0xFF),
                static_cast<uint8_t>(batteryPercent & 0xFF),
        };
        LoRa::tx(buffer, SIZE);
    }

}