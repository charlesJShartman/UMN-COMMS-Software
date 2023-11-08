#ifndef _RADIO_DRIVER_
#define _RADIO_DRIVER_

/**
 * Interface for how radio drivers should be implemented
 * This class is used as a header for the radio_service.cpp,
 *   but the satellite radio driver or ground radio driver are
 *   used for actual executables.
 */

#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <cstddef>

#include "comms_hardware.h"
#include "packet/ppp_packet.h"

class RadioDriverException : public std::exception {
    std::string what_;
  public:
    RadioDriverException(std::string what) : what_(what) {}
    const char* what() const throw() {
        return what_.c_str();
    }
};

class RadioDriver {
  protected:
    // debug settings
    static constexpr bool VERBOSE_CONSTRUCTOR = true;
    static constexpr uint8_t VERBOSE_PULL_FROM_RADIO = 0;
    static constexpr uint8_t VERBOSE_PUSH_TO_RADIO = 0;

    // internal radio buffer. Note that the radio's actual buffer is 512 bytes.
    static constexpr size_t MAX_RADIO_BUFFER_SIZE = 512;
    std::byte radio_buf_[MAX_RADIO_BUFFER_SIZE];
    size_t  radio_buf_pos_ = 0;
    size_t  radio_buf_size_ = 0;

    RadioDriver();
  public:

    virtual void pushPacketToRadio(const ppp::Packet& wrapped_packet) = 0;
    ppp::Packet pullPacketFromRadio();

    size_t bytesInBuffer();
    virtual void refillRadioBuffer() = 0;

    virtual void sleep(unsigned int seconds) = 0;

    virtual bool getCD() = 0;
    virtual int getRadioFD() = 0;
  protected:
    std::byte pullByteFromRadio();
};

#endif
