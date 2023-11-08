#include "packet/radio_service.h"

namespace comms {
    template <typename RadioDriver>
    RadioService<RadioDriver>::RadioService() {
        fd_manager_.addFileDescriptor(real_time_socket_.get_socket_file_descriptor());
        fd_manager_.addFileDescriptor(reliable_socket_.get_socket_file_descriptor());
        fd_manager_.addFileDescriptor(radio_driver_.getRadioFD());

        sleep(3);
        run();
    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::run() {
        if (verbose)
            std::cout << "RadioService::run()" << std::endl;

        running = true;
        while (running) {
            usleep(500000);

            try {
                idle();
                active();
            } catch (const std::exception &e) {
                if (verbose)
                    std::cout << e.what() << std::endl;
            }

        }
    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::idle() {
        if (!radio_driver_.getCD())
            std::cout << std::endl << "Entering idle state" << std::endl;
        while (!radio_driver_.getCD() && running) {

            flushSocket(real_time_socket_);

            radio_driver_.sleep(IDLE_SLEEP_TIME);
            sleep(IDLE_AWAKE_TIME);
        }
    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::active() {
        if (radio_driver_.getCD())
            std::cout << std::endl << "Entering active state" << std::endl;
        while (radio_driver_.getCD() && running) {
            while (radio_driver_.bytesInBuffer())
                processFromRadioDriver();

            int status = fd_manager_.wait();
            if (status == 0)
                continue;

            //Check radio
            if (fd_manager_.areBytesWaiting(radio_driver_.getRadioFD()))
                processFromRadioDriver();

            //Check real_time_sockfd
            else if (fd_manager_.areBytesWaiting(real_time_socket_.get_socket_file_descriptor()))
                processFromSocket(real_time_socket_);

            //Check reliable_sockfd
            else if (fd_manager_.areBytesWaiting(reliable_socket_.get_socket_file_descriptor()))
                processFromSocket(reliable_socket_);
        }
    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::processFromRadioDriver() {

        try {
            ppp::Packet wrapped_packet {radio_driver_.pullPacketFromRadio()};

            try {
                stats::packets_received++;
                if (stats::packets_received % 100 == 0)
                    std::cout << std::endl;
                udp::Packet unwrapped_packet {wrapped_packet.unwrap()};
                sendUDPPacket(unwrapped_packet);
                stats::good_packets++;
                if (verbose) {
                    std::cout << "Sending packet: " << unwrapped_packet << std::endl;
                } else {
                    if (stats::packets_received % 10 == 0)
                        std::cout << '>';
                    else
                        std::cout << '.';
                    fflush(stdout);
                }
            } catch (const ppp::BadCRC& e) {
                stats::bad_crcs++;
                if (verbose)
                    std::cout << "[ERROR] " << e.what() << std::endl;
                else
                    std::cout << 'C';

            } catch (const ppp::ShortPacket& e) {
                stats::short_packets++;
                if (verbose)
                    std::cout << "[ERROR] " << e.what() << std::endl;
                else
                    std::cout << 'S';
            }
        } catch (const RadioDriverException& e) {
            if (verbose)
                std::cout << e.what() << std::endl;
            return;
        }

    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::processFromSocket(udp::Socket& socket) {
        ppp::Packet wrapped_packet {socket.receive_packet()};
        radio_driver_.pushPacketToRadio(wrapped_packet);
        stats::packets_sent++;
    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::flushSocket(udp::Socket& socket) {
        while (fd_manager_.wait(0))
            if (fd_manager_.areBytesWaiting(socket.get_socket_file_descriptor()))
                socket.receive_packet();
            else
                return;
    }

    template <typename RadioDriver>
    int RadioService<RadioDriver>::sendUDPPacket(const udp::Packet& packet) {
        udp::PacketHeader header = packet.getHeader();
        uint16_t dest = header.destination_port;
        if (dest == real_time_socket_.getPort() || dest == reliable_socket_.getPort()) {
            if (verbose)
                std::cerr << "[ERROR]: packet destination port is a radio port" << std::endl;
            return -1;
        }

        if (verbose)
            std::cout << "Sending packet to port " << dest << std::endl;

        subsystem_address::location_t dest_location = {
            .port = dest,
            .address = "127.0.0.1"};

        int status = real_time_socket_.send_packet(packet, dest_location);
        return status;
    }

    template <typename RadioDriver>
    void RadioService<RadioDriver>::shutdown() {
        running = false;
    }

} // namespace comms

void signalHandler(int signum) {
    std::cout << std::endl << "Signal " << signum << " Received. Exiting... " << std::endl << std::endl;
    comms::stats::print();
    comms::running = false;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <driver> <verbose>" << std::endl;
        std::cerr << "       <driver> is either 'sat' or 'gnd' to indicate which driver is used" << std::endl;
        std::cerr << "       <verbose> is either 'verbose' or omitted to indicate if extra information should be printed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string driver {argv[1]};

    if (argc > 2)
        if (strcmp(argv[2], "verbose") == 0)
            comms::verbose = true;

    SignalManager signal_manager {signalHandler, SIGINT};

    if (driver == "sat")
        comms::RadioService<SatelliteRadioDriver> radio_service {};
    else if (driver == "gnd")
        comms::RadioService<GroundRadioDriver> radio_service {};
    else {
        std::cerr << "Invalid driver" << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
