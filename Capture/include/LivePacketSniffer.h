#pragma once

#include "IPacketSniffer.h"


class LivePacketSniffer : public IPacketSniffer {
    private:
        std::string subjectName;
        pcap_if_t* allDevs = nullptr;
        void printAllInterfaces();  
    public:
        LivePacketSniffer(NATSJetstream& pub) : IPacketSniffer(pub) {}
        void livePacketHandler(u_char* user, const pcap_pkthdr* header, const u_char* packet);
        void run(const std::string& SUBJECT_NAME) override;
};
