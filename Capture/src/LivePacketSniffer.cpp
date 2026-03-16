#include "CaptureConfig.h"
#include "LivePacketSniffer.h"
#include "SIPPacketHandler.h"
#include "SIPPacketSerializer.h"
#include "TimestampFormatter.h"
#include "LogMacros.h"


void LivePacketSniffer::printAllInterfaces() {
    LOG_TRACE("Attempting to list all network interfaces.");
    if (pcap_findalldevs(&allDevs, errbuf) == -1) {
        LOG_INFO("Error finding devices: {}", errbuf);
        std::cerr << "Error finding devices: " << errbuf << std::endl;
        return;
    }

    std::cout << "Available network interfaces:\n";
    int i = 0;
    for (pcap_if_t* d = allDevs; d != nullptr; d = d->next) {
        LOG_TRACE("Found interface: {}", d->name);
        std::cout << i++ << ": " << d->name;
        if (d->description)
            std::cout << " (" << d->description << ")";
        std::cout << std::endl;
    }

    if (i == 0){
        LOG_WARN("No interfaces found on this machine.");
        std::cerr << "No interfaces found!" << std::endl;
    }
        
    pcap_freealldevs(allDevs); // Free device list after use
}

static void packetHandlerAdapter(u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {
    auto* sniffer = reinterpret_cast<LivePacketSniffer*>(user);
    sniffer->livePacketHandler(user, header, packet);
}

void LivePacketSniffer::livePacketHandler(u_char* user, const pcap_pkthdr* header, const u_char* packet) {
    SIPPacketHandler packetHandler;
    SIPPacketSerializer serializer;
    TimestampFormatter tsFormatter;

    if (packetHandler.isRelevantPacket(header, packet)) {
        SIPPacket sipPacket;
        sipPacket.sequenceNo = static_cast<uint64_t>(header->ts.tv_sec) * CaptureConfig::microsecondMultiplier + header->ts.tv_usec;
        sipPacket.packetData.assign(packet + CaptureConfig::ethernetHeaderSize, packet + header->caplen);
        sipPacket.arrivalTime = tsFormatter.getFormattedTimestamp(const_cast<timeval&>(header->ts));

        LOG_TRACE("Relevant LIVE SIP packet found with Sequence Number: {}, Arrival Time: {}, Packet Size: {}", 
            sipPacket.sequenceNo, sipPacket.arrivalTime, sipPacket.packetData.size());

        std::vector<uint8_t> serializedData = serializer.serializePacket(sipPacket);

        if (!publisher.publishMessage(CaptureConfig::SUBJECT_NAME, serializedData)) {
            LOG_ERROR("Failed to publish LIVE packet with Sequence Number: {}", sipPacket.sequenceNo);
        } else {
            LOG_INFO("Published LIVE SIP Packet with Packet Size: {} bytes on subject: *{}*",
                    serializedData.size(), CaptureConfig::SUBJECT_NAME);
        }
    } else {
        LOG_DEBUG("LIVE Packet ignored (not a relevant packet).");
    }
}


void LivePacketSniffer::run(const std::string& SUBJECT_NAME) {
    LOG_INFO("Starting LivePacketSniffer on subject: {}", SUBJECT_NAME);

    subjectName = SUBJECT_NAME;
    printAllInterfaces();  // Optional

    LOG_INFO("Attempting to open interface: {}", CaptureConfig::interfaceName);

    pcap = pcap_open_live(CaptureConfig::interfaceName.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (pcap == nullptr) {
        LOG_ERROR("Could not open device: {}", CaptureConfig::interfaceName);
        return;
    } else {
        LOG_INFO("Successfully opened device: {}", CaptureConfig::interfaceName);
        std::cout << "\nListening on interface: " << CaptureConfig::interfaceName << std::endl;
    }

    LOG_INFO("Starting live capture loop...");

    // Pass "this" pointer as user data so we can use member function
    pcap_loop(pcap, 0, packetHandlerAdapter, reinterpret_cast<u_char*>(this));

    LOG_INFO("Capture loop ended. Closing device.");

    pcap_close(pcap);
}
