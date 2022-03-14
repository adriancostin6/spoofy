#include "capture.h"

#include <iostream>
#include <exception>
#include <functional>

namespace nped {
PacketSniffer::PacketSniffer(SnifferType st,
                             const char *iface,
                             const char *ptype) { 
    setup(st, iface, ptype);
}

void PacketSniffer::setup(SnifferType st, const char *iface, const char *ptype) {
    Tins::SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter(ptype);

    try {
        if (st == SnifferType::FileSniffer) {
            sniffer = std::make_unique<Tins::Sniffer>(iface, config);
        } else {
            sniffer = std::make_unique<Tins::FileSniffer>(iface, config);
        }
    } catch (std::exception &ex) {
        std::cerr << "Error:" << ex.what() << std::endl;
    }
}

bool PacketSniffer::callback(Tins::PDU& pdu) { 
    std::cout << "Hello, world\n";
    return true;
}

void PacketSniffer::run_sniffer() {
    try {
        sniffer->sniff_loop(std::bind(
                               &PacketSniffer::callback,
                               this,
                               std::placeholders::_1));
    } catch (std::exception &ex) {
        std::cerr << "Error:" << ex.what() << std::endl;
    }
}
}
