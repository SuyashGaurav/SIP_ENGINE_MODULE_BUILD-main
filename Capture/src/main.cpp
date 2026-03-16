#include <iostream>
#include <thread>
#include <string>

#include "CaptureConfig.h"
#include "FilePacketSniffer.h"
#include "LivePacketSniffer.h"
#include "NATSJetStream.h"
#include "LogMacros.h"

void runFileSniffer(NATSJetstream& jetstream) {
    FilePacketSniffer fileSniffer(jetstream);
    fileSniffer.run(CaptureConfig::SUBJECT_NAME);
}

void runLiveSniffer(NATSJetstream& jetstream) {
    LivePacketSniffer liveSniffer(jetstream);
    liveSniffer.run(CaptureConfig::SUBJECT_NAME);
}

int main() {
    try {
        Logger::init();
        LOG_INFO("Capture Module Started");
        CaptureConfig::loadFromINIFile("../configs/CaptureConfig.ini");
        LOG_INFO("Capture Configuration loaded from INI file");

        NATSJetstream jetstream;
        if(jetstream.connectToNATS(CaptureConfig::natsURL) != 0) return 1;
        if(jetstream.createJetStream() != 0) return 1;
        if(jetstream.createStream(CaptureConfig::STREAM_NAME,
                    CaptureConfig::SUBJECT_NAME) != 0) return 1;

        std::string choice = CaptureConfig::packetSnifferMode;
        std::cout << choice << " Packet Sniffer Mode selected.\n";

        if (choice == "File") {
            runFileSniffer(jetstream);
        } else if (choice == "Live") {
            runLiveSniffer(jetstream);
        } else if (choice == "Both") {
            std::thread fileThread(runFileSniffer, std::ref(jetstream));
            std::thread liveThread(runLiveSniffer, std::ref(jetstream));
            fileThread.join();
            liveThread.join();
        } else {
            std::cerr << "Invalid choice.\n";
        }

        jetstream.NATSCleanUp();
    } catch (const std::exception& e) {
        LOG_ERROR("Fatal error: {}", e.what());
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
