#include "packet/driver/ground_radio_driver.h"
#include "comms_hardware.h"

GroundRadioDriver::GroundRadioDriver() {
    if (VERBOSE_CONSTRUCTOR)
        std::cout << "Using Ground Radio Driver" << std::endl;

    sendCommand(comms::commands::ACTIVE_MODE);
    sendCommand(comms::commands::CONNECT);
}

GroundRadioDriver::~GroundRadioDriver() {
    sendCommand(comms::commands::IDLE_MODE);
}

void GroundRadioDriver::pushPacketToRadio(const ppp::Packet& wrapped_packet) {

    if (VERBOSE_PUSH_TO_RADIO >=1)
        std::cout << "GroundRadioDriver::pushPacketToRadio" << std::endl;

    ssize_t ret_val;
    size_t bytes_written = 0;

    while (bytes_written < wrapped_packet.data_.size()) {

        size_t num_to_write = wrapped_packet.data_.size() - bytes_written;
        if (num_to_write > 200)
            num_to_write = 200;

        ret_val = data_serial_.write(wrapped_packet.data_.data() + bytes_written, num_to_write);

        if (ret_val > 0)
            bytes_written += ret_val;
    }

    if (VERBOSE_PUSH_TO_RADIO >= 2)
        std::cout << wrapped_packet << std::endl;
}

void GroundRadioDriver::refillRadioBuffer() {
    if (bytesInBuffer())
        return;
    int bytes_read = data_serial_.read(radio_buf_, MAX_RADIO_BUFFER_SIZE);
    radio_buf_pos_ = 0;
    radio_buf_size_ = bytes_read;
}

void GroundRadioDriver::sleep(unsigned int seconds) {
    ::sleep(seconds);
}

bool GroundRadioDriver::getCD() {
    return true;
}

int GroundRadioDriver::getRadioFD() {
    return data_serial_.getFD();
}

void GroundRadioDriver::sendCommand(const uint8_t command[]) {
    const size_t RESPONSE_LENGTH {32};
    uint8_t response[RESPONSE_LENGTH] {};

    if (command_serial_.write(command, size_t {command[0]} + 1) <= 0)
        throw RadioDriverException("Failed to write to the command port");

    for (int i = 0; i < 10; i++) {
        usleep(500000);
        if (command_serial_.read(response, RESPONSE_LENGTH) > 0)
            return;
    }

    throw RadioDriverException("Response not received from command port");
}
