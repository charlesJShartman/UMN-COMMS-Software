#include "packet/driver/radio_driver.h"

RadioDriver::RadioDriver() {}

ppp::Packet RadioDriver::pullPacketFromRadio() {

    if (VERBOSE_PULL_FROM_RADIO >=1)
        std::cout << "RadioDriver::pullPacketFromRadio" << std::endl;

    std::vector<std::byte> packet;

    // Find first flag
    try {
        packet.push_back(pullByteFromRadio());
        while (packet.back() != ppp::FLAG) {
            if (packet.back() == ppp::ESC)
                packet.back() = pullByteFromRadio();
            packet.back() = pullByteFromRadio();
        }
    } catch (const RadioDriverException &e) {
        throw RadioDriverException("Noise");
    }

    // verify it is first flag
    packet.push_back(pullByteFromRadio());
    if (packet.back() == ppp::FLAG)
        packet.pop_back();

    // Find next flag
    do {
        if (packet.back() == ppp::ESC)
            packet.push_back(pullByteFromRadio());
        packet.push_back(pullByteFromRadio());
    } while (packet.back() != ppp::FLAG);

    //ppp::Packet output {std::move(packet)};
    //if (VERBOSE_PULL_FROM_RADIO >= 2)
    //    std::cout << output << std::endl;

    return {std::move(packet)};
}

size_t RadioDriver::bytesInBuffer() {
    return radio_buf_size_ - radio_buf_pos_;
}

std::byte RadioDriver::pullByteFromRadio() {
    if (radio_buf_pos_ >= radio_buf_size_) {
        refillRadioBuffer();

        //If there is still no data in the radio buffer
        if (radio_buf_size_ == 0) {
            usleep(100000);
            refillRadioBuffer();
            if (radio_buf_size_ == 0)
                throw RadioDriverException("No more bytes in UART buffer");
        }
    }
    return radio_buf_[radio_buf_pos_++];
}
