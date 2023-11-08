#include "packet/ppp_packet.h"

namespace ppp {

    Packet::Packet(const udp::Packet& unwrapped_packet) : data_() {
        // Create new packet data
        data_.reserve(unwrapped_packet.data_.size() * 1.1);
        data_.push_back(FLAG);
        uint16_t crc {0xffff};
        uint8_t temp {};

        // Loop through each character
        for (std::byte c : unwrapped_packet.data_) {
            // Escape flags and escape characters
            if (c == FLAG || c == ESC)
                data_.push_back(ESC);
            data_.push_back(c);

            // Calculate CRC
            uint8_t c_int = static_cast<uint8_t>(c);
            temp = c_int ^ (crc >> 8);
            crc = crc_table[temp] ^ (crc << 8);
        }

        // Append CRC and flag
        std::byte crc_byte_1 {static_cast<std::byte>(crc >> 8)};
        std::byte crc_byte_2 {static_cast<std::byte>(crc)};

        if (crc_byte_1 == FLAG || crc_byte_1 == ESC)
            data_.push_back(ESC);
        data_.push_back(crc_byte_1);

        if (crc_byte_2 == FLAG || crc_byte_2 == ESC)
            data_.push_back(ESC);
        data_.push_back(crc_byte_2);

        data_.push_back(FLAG);
    }

    Packet::Packet(const std::vector<std::byte>&& wrapped_packet) : data_(std::move(wrapped_packet)) {
        if (data_.size() < 4 || data_.front() != FLAG || data_.back() != FLAG)
            throw ShortPacket("Packet is not a complete PPP packet");
    }

    udp::Packet Packet::unwrap() const {
        // Verify packet is in  form
        if (data_.size() < 4 || data_.front() != FLAG || data_.back() != FLAG) {
            throw ShortPacket("Packet is not a complete PPP packet");
        }

        // Create new packet data
        std::vector<std::byte> new_data {};
        new_data.reserve(data_.size() - 4);
        uint16_t crc = 0xffff;
        uint8_t temp;

        size_t i = 1;
        while (i < data_.size() - 1) {
            // Remove escape characters
            if (data_[i] == ESC)
                i++;

            std::byte c = data_[i];
            new_data.push_back(c);

            // Calculate CRC
            uint8_t c_int = static_cast<uint8_t>(c);
            temp = c_int ^ (crc >> 8);
            crc = crc_table[temp] ^ (crc << 8);
            i++;
        }
        new_data.pop_back();
        new_data.pop_back();

        // Update packet
        if (crc == 0)
            return udp::Packet(new_data);

        throw BadCRC("Packet corrupted");
    }

    std::ostream& operator<<(std::ostream& os, ppp::Packet const& packet) {
        os << "0x";

        char hex_map[] {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        std::byte first_byte {0xF0};
        std::byte second_byte {0x0F};

        for (std::byte b : packet.data_) {
            os << hex_map[static_cast<int>((b & first_byte) >> 4)];
            os << hex_map[static_cast<int>(b & second_byte)];
        }

        return os;
    }
}
