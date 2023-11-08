#ifndef _SATELLITE_RADIO_DRIVER_
#define _SATELLITE_RADIO_DRIVER_

#include "radio_driver.h"
#include "util/raii_gpio.h"
#include "util/uart.h"

class SatelliteRadioDriver : public RadioDriver {

    // for cts and cd, low means they are asserted
    RaiiGpioPin cts_ {comms::pins::CTS, RaiiGpioPin::Operation::read};
    RaiiGpioPin adv_awake_ {comms::pins::ADV_AWAKE, RaiiGpioPin::Operation::write};
    RaiiGpioPin cd_ {comms::pins::CD, RaiiGpioPin::Operation::read};

    UART serial_ {comms::ports::SATELLITE_DATA_PORT, UART::emptySettings(comms::BAUDRATE)};

  public:

    SatelliteRadioDriver();

    SatelliteRadioDriver(const SatelliteRadioDriver &other) = delete;
    SatelliteRadioDriver& operator=(const SatelliteRadioDriver &other) = delete;

    void pushPacketToRadio(const ppp::Packet& wrapped_packet);

    void refillRadioBuffer();

    void sleep(unsigned int seconds);

    bool getCD();

    int getRadioFD();
  private:
    void assertCD();
};

#endif
