#include "packet/driver/satellite_radio_driver.h"

SatelliteRadioDriver::SatelliteRadioDriver() {
    if (VERBOSE_CONSTRUCTOR)
        std::cout << "Using Satellite Radio Driver" << std::endl;

    // toggle ADV_AWAKE to wake up radio
    adv_awake_.write(false);
    usleep(10000);
    adv_awake_.write(true);
}


void SatelliteRadioDriver::pushPacketToRadio(const ppp::Packet& wrapped_packet) {

    if (VERBOSE_PUSH_TO_RADIO >=1)
        std::cout << "SatelliteRadioDriver::pushPacketToRadio" << std::endl;

    ssize_t ret_val;
    size_t bytes_written = 0;

    while (bytes_written < wrapped_packet.data_.size()) {
        assertCD();

        size_t num_to_write = wrapped_packet.data_.size() - bytes_written;
        if (num_to_write > 200)
            num_to_write = 200;

        ret_val = serial_.write(wrapped_packet.data_.data() + bytes_written, num_to_write);

        if (ret_val > 0)
            bytes_written += ret_val;
    }

    if (VERBOSE_PUSH_TO_RADIO >= 2)
        std::cout << wrapped_packet << std::endl;
}


void SatelliteRadioDriver::refillRadioBuffer() {
    if (bytesInBuffer())
        return;
    size_t bytes_read = serial_.read(radio_buf_, MAX_RADIO_BUFFER_SIZE);
    radio_buf_pos_ = 0;
    radio_buf_size_ = bytes_read;
}

void SatelliteRadioDriver::sleep(unsigned int seconds) {
    adv_awake_.write(false);
    ::sleep(seconds);
    adv_awake_.write(true);
}

bool SatelliteRadioDriver::getCD() {
    return !(cd_.read());
}

int SatelliteRadioDriver::getRadioFD() {
    return serial_.getFD();
}

////////////////////////////////////////////////////////////////////////////////
//// PRIVATE METHODS ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void SatelliteRadioDriver::assertCD() {
    if (cts_.read() || cd_.read()) {
        for (int i = 0; i < 100; i++) {
            usleep(10000);
            if (!(cts_.read()) && !(cd_.read()))
                return;
        }
        throw RadioDriverException("CTS/CD Lost");
    }
}
