#ifndef _TELEM_H_
#define _TELEM_H_

#include "lora/lora.h"

namespace Telem {

    enum class Command {
        NONE = 0,
        ACK = 1,
        GET_STATUS = 2,
        STATUS = 3,
        OPEN = 4,
        CLOSE = 5,
    };

    bool init();
    Command getCommand(uint8_t launcherID);
    void sendCommand(uint8_t launcherID, Command command);
    void sendAck(uint8_t launcherID, Command command);
    void sendStatus(uint8_t launcherID, bool hatchOpen, float altitude, float temperature, int batteryPercent);

}

#endif