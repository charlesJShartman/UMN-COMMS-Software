#ifndef _RADIO_SERVICE_
#define _RADIO_SERVICE_

#include "udp/socket.h"
#include "udp/udp.h"
#include "ppp_packet.h"
#include "util/file_descriptor_manager.h"
#include "util/signal_manager.h"

#include "driver/radio_driver.h"
#include "driver/ground_radio_driver.h"
#include "driver/satellite_radio_driver.h"

#include "comms_stats.h"

namespace comms {
    bool running {};
    bool verbose {};

    constexpr int IDLE_SLEEP_TIME {14};
    constexpr int IDLE_AWAKE_TIME {2};

    template <typename RadioDriver>
    class RadioService {

        udp::Socket real_time_socket_ {subsystem_address::kcomsService};
        udp::Socket reliable_socket_ {subsystem_address::kcomsServiceReliable};

        RadioDriver radio_driver_ {};

        FileDescriptorManager fd_manager_ {};

      public:

        /**
         * @brief Creates a new RadioService object
         *
         * @return RadioService
         */
        RadioService();

      private:

        /**
         * @brief Starting point for the service
         */
        void run();

        /**
         * @brief Ends the run loop if it is running
         */
        void shutdown();

        void idle();

        void active();

        void processFromRadioDriver();

        void processFromSocket(udp::Socket& socket);

        void flushSocket(udp::Socket& socket);

        /**
         * @brief Sends a UDP packet to the port given in its header
         *
         * @param packet udp::Packet to be sent
         *
         * @return Status of packet sent
         */
        int sendUDPPacket(const udp::Packet& packet);

    };
}

#endif
