#pragma once

#include "IPacketSniffer.h"

class FilePacketSniffer : public IPacketSniffer {
    private:
        void openPcapFile(const std::string& filename);
    public:
        FilePacketSniffer(NATSJetstream& pub) : IPacketSniffer(pub) {}
        void run(const std::string& SUBJECT_NAME) override;
};

