#ifndef _COMMS_HARDWARE_
#define _COMMS_HARDWARE_

#include <string>
#include <stdint.h>
#include <termios.h>

namespace comms {
    namespace ports {
        inline const std::string SATELLITE_DATA_PORT {"/dev/serial0"};
        inline const std::string GROUND_DATA_PORT    {"/dev/ttyUSB0"};
        inline const std::string GROUND_COMMAND_PORT {"/dev/ttyUSB1"};
    }

    enum pins : uint8_t {
        CTS = 16,
        RTS = 17,
        ADV_AWAKE = 42,
        MAIN_PWR_RESET = 43,
        INT = 44,
        CD = 45
    };

    inline const speed_t BAUDRATE {B921600};
    inline const speed_t COMMAND_BAUDRATE {B115200};

    namespace commands {
        inline const uint8_t IDLE_MODE[] {2, 101, 0};
        inline const uint8_t ACTIVE_MODE[] {2, 101, 1};
        inline const uint8_t CONNECT[] {14, 105, 1, 1, 11, 125, 0, 0, 8, 0, 0, 0, 0, 0, 0};
    }
}

#endif
