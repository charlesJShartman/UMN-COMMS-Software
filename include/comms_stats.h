#ifndef _COMMS_STATS_
#define _COMMS_STATS_

#include <iostream>

namespace comms {
    namespace stats {
        static size_t packets_sent {};

        static size_t packets_received {};
        static size_t good_packets {};
        static size_t bad_crcs {};
        static size_t short_packets {};

        inline void print() {
            std::cout << "    Total Packets Sent: " << stats::packets_sent << std::endl;
            std::cout << std::endl;
            std::cout << "Total Packets Received: " << stats::packets_received << std::endl;
            std::cout << "          Good Packets: " << stats::good_packets << std::endl;
            std::cout << "              Bad CRCs: " << stats::bad_crcs << std::endl;
            std::cout << "         Short Packets: " << stats::short_packets << std::endl;
        }
    }
}

#endif
