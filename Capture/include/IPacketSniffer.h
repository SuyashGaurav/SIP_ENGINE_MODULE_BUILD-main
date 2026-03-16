#pragma once

#include "NATSJetstream.h"
#include <iostream>
#include <pcap.h>
#include <string>

class IPacketSniffer {
    protected:
        struct pcap_pkthdr* header;
        const u_char* packet;
        pcap_t* pcap;
        NATSJetstream& publisher;
        char errbuf[PCAP_ERRBUF_SIZE];
    public:
        IPacketSniffer(NATSJetstream& pub) : publisher(pub) {}
        virtual void run(const std::string& SUBJECT_NAME) = 0;
        virtual ~IPacketSniffer() {}
};
