#include "FilePacketSniffer.h"
#include "SIPPacketHandler.h"
#include "SIPPacketSerializer.h"
#include "TimestampFormatter.h"
#include "CaptureConfig.h"
#include "LogMacros.h"
#include <filesystem>

namespace fs = std::filesystem;

void FilePacketSniffer::openPcapFile(const std::string& filename) {
    pcap = pcap_open_offline(filename.c_str(), errbuf);
    if (!pcap) {
        LOG_ERROR("Error opening pcap file: {}", errbuf);
    }
}

void FilePacketSniffer::run(const std::string& SUBJECT_NAME) {
    SIPPacketHandler sipPacketHandler;
    SIPPacketSerializer sipPacketSerializer;
    TimestampFormatter timestampFormatter;

    for (const auto& entry : fs::directory_iterator(CaptureConfig::inputFolderPath)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            std::string ext = entry.path().extension().string();

            if (ext == ".pcap" || ext == ".pcapng") {
                LOG_INFO("Processing file: {}", filepath);
                std::cout << "Processing file: " << filepath << std::endl;
                openPcapFile(filepath);
                if (!pcap) {
                    LOG_ERROR("Failed to open pcap file: {}", filepath);
                    continue;
                }

                int frameNumber = 1;

                while (pcap_next_ex(pcap, &header, &packet) == 1) {
                    if (sipPacketHandler.isRelevantPacket(header, packet)) {

                        LOG_DEBUG("Frame {}: Relevant SIP packet found", frameNumber);

                        SIPPacket sipPacket;
                        sipPacket.sequenceNo = static_cast<uint64_t>(header->ts.tv_sec) * CaptureConfig::microsecondMultiplier + header->ts.tv_usec;
                        sipPacket.packetData.assign(packet + CaptureConfig::ethernetHeaderSize, packet + header->caplen);
                        sipPacket.arrivalTime = timestampFormatter.getFormattedTimestamp(header->ts);

                        LOG_TRACE("Packet Details: Frame Number: {}, Arrival Time: {}, Packet Size: {} bytes",
                                 frameNumber, sipPacket.arrivalTime, sipPacket.packetData.size());

                        std::vector<uint8_t> serializedData = sipPacketSerializer.serializePacket(sipPacket);

                        if (!publisher.publishMessage(CaptureConfig::SUBJECT_NAME, serializedData)) {
                            LOG_ERROR("Failed to publish packet with Frame Number: {}, In File: {}", frameNumber, filepath);
                        } else {
                            LOG_INFO("Published SIP Packet with Frame Number: {}, Packet Size: {} bytes on subject: *{}*",
                                     frameNumber, serializedData.size(), CaptureConfig::SUBJECT_NAME);
                        }
                    }
                    else {
                        LOG_DEBUG("Frame {}: In File {}, Packet not relevant, skipping.", frameNumber, filepath);
                    }
                    frameNumber++;
                }

                pcap_close(pcap);
                pcap = nullptr; // reset for next file
            }
        }
    }
}
