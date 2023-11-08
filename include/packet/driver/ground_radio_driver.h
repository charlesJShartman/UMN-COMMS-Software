#ifndef _GROUND_RADIO_DRIVER_
#define _GROUND_RADIO_DRIVER_

#include "comms_hardware.h"
#include "radio_driver.h"
#include "util/uart.h"

class GroundRadioDriver : public RadioDriver {
    UART data_serial_ {comms::ports::GROUND_DATA_PORT, UART::emptySettings(comms::BAUDRATE)};
    UART command_serial_ {comms::ports::GROUND_COMMAND_PORT,
        UART::disableHardwareFlowControl(UART::emptySettings(comms::COMMAND_BAUDRATE))};

  public:

    GroundRadioDriver();
    ~GroundRadioDriver();

    GroundRadioDriver(const RadioDriver &other) = delete;
    GroundRadioDriver& operator=(const RadioDriver &other) = delete;

    void pushPacketToRadio(const ppp::Packet& wrapped_packet);

    void refillRadioBuffer();

    void sleep(unsigned int seconds);

    bool getCD();

    int getRadioFD();

  private:

    void sendCommand(const uint8_t command[]);
};

#endif
