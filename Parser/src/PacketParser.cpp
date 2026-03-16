#include <iostream>
#include <regex>
#include <nats/nats.h>
#include "PacketParser.h"
#include "ParsedDataSerializer.h"
#include "ParserConfig.h"
#include "PriorityQueue.h"
#include "PacketDeserializer.h"
#include "TransactionMessage.h"
#include "NATSJetstream.h"
#include "LogMacros.h"
#include "Request.h"
#include "Response.h"

bool PacketParser::isSipRequest(const std::string &sipMessage){
    std::string firstLine;
    std::istringstream stream(sipMessage);
    std::getline(stream, firstLine);

    // Remove leading spaces
    firstLine.erase(0, firstLine.find_first_not_of(" \t"));

    // Check if the first line starts with "SIP/"
    if (firstLine.rfind("SIP/", 0) == 0) { // Starts with "SIP/"
        return false; // It's a response
    }
    return true; // It's a request
}

std::string PacketParser::getSipHeader(const std::string &sipMessage, const std::string &headerName) {
    std::regex headerRegex(headerName + R"(:\s*(.+?)(;|\r\n|\n))");
    std::smatch match;
    if (std::regex_search(sipMessage, match, headerRegex)) {
        return match[1].str();
    }
    return "";
}

std::string PacketParser::extractSipMethod(const std::string &sipMessage) {
    size_t end = sipMessage.find(" ");
    return sipMessage.substr(0, end);
}

std::string PacketParser::extractStatus(const std::string &sipMessage){
    std::regex responseRegex(R"(SIP/\d\.\d\s+(\d+)\s+(.+))");
    std::smatch match;

    if (std::regex_search(sipMessage, match, responseRegex)) {
        return match[1].str()+" "+match[2].str();  // Extracted response phrase
    }

    return "";
}

int PacketParser::extractStatusCode(const std::string &status){
    std::regex responseRegex(R"(SIP/\d\.\d\s+(\d+)\s+.+)");
    std::smatch match;

    if (std::regex_search(status, match, responseRegex)) {
        return std::stoi(match[1].str());  // Only the status code
    }

    return 0;

}
//This is to be filled after the configuration file is set up
std::string PacketParser::extractSourceIPAddress(SIPPacket packet){
    char srcIP[16];
    int srcIpStartingIndex=ParserConfig::srcIpStartingIndex;
    snprintf(srcIP, sizeof(srcIP), "%d.%d.%d.%d",
            packet.packetData[srcIpStartingIndex], packet.packetData[srcIpStartingIndex+1], packet.packetData[srcIpStartingIndex+2], packet.packetData[srcIpStartingIndex+3]);
    return srcIP;
    
}

std::string PacketParser::extractDestinationIPAddress(SIPPacket packet){
    char destIP[16];
    int destIpStartingIndex=ParserConfig::destIpStartingIndex;
    snprintf(destIP, sizeof(destIP), "%d.%d.%d.%d",
            packet.packetData[destIpStartingIndex], packet.packetData[destIpStartingIndex+1], packet.packetData[destIpStartingIndex+2], packet.packetData[destIpStartingIndex+3]);
    return destIP;

}

std::optional<std::map<std::string, SDP>> PacketParser::extractSDP(const std::string &sipMessage) {
    std::map<std::string, SDP> sdpInfo;
    std::regex mediaRegex(R"(m=(\w+)\s+(\d+)\s+(\S+)\s+([\d\s]+))");
    std::regex connRegex(R"(c=([\w]+)\s+(IP4|IP6)\s+([\d\.]+))");
    std::regex attributeRegex(R"(a=([\w\-]+)(?::(.*))?)");

    std::smatch match;
    std::istringstream sdpStream(sipMessage);
    std::string line;
    std::string currentMedia;
    ConnectionInformation globalConnInfo;

    while (std::getline(sdpStream, line)) {
        if (std::regex_search(line, match, connRegex)) {
            std::string netType = match[1];
            std::string addrType = match[2];
            std::string addr = match[3];

            if (currentMedia.empty()) {
                globalConnInfo.networkType = netType;
                globalConnInfo.addressType = addrType;
                globalConnInfo.address = addr;
            } else {
                auto &conn = sdpInfo[currentMedia].connectionInfo;
                conn.networkType = netType;
                conn.addressType = addrType;
                conn.address = addr;
            }
        }
        else if (std::regex_search(line, match, mediaRegex)) {
            currentMedia = match[1];
            SDP &sdp = sdpInfo[currentMedia];
            sdp.mediaType = match[1];
            sdp.mediaPort = std::stoi(match[2]);
            sdp.protocol = match[3];

            std::istringstream codecStream(match[4]);
            std::string codec;
            while (codecStream >> codec) {
                sdp.codecs.push_back(codec);
            }

            // Use global connection info if no per-media is provided
            sdp.connectionInfo = globalConnInfo;
        }
        else if (std::regex_search(line, match, attributeRegex) && !currentMedia.empty()) {
            Attributes attr;
            attr.name = match[1];
            attr.value = match[2].matched ? match[2].str() : "";
            sdpInfo[currentMedia].attributes.push_back(attr);
        }
    }

    return sdpInfo.empty() ? std::nullopt : std::optional(sdpInfo);
}

std::string PacketParser::extractExtension(const std::string &headerValue) {
    std::regex extensionRegex(R"(<sip:(\d+)@)");  // Extracts numbers in <sip:201@...>
    std::smatch match;
    if (std::regex_search(headerValue, match, extensionRegex)) {
        return match[1].str();
    }
    return "";
}

std::unique_ptr<TransactionMessage> PacketParser::parseSipPacket(SIPPacket packet){
    
    //Determine the protocol
    int protocol = (int)packet.packetData[9];

    //Determine the IPHeader Length
    uint8_t ipHeaderLength = (packet.packetData[0] & 0x0F) * 4;

    //Extract the basic Transaction related Details
    TransactionMessage transaction;
    transaction.parsedData.seqNo=packet.sequenceNo;
    transaction.parsedData.sipMessage.timestamp=packet.arrivalTime;
    transaction.parsedData.protocol=protocol;
   
    transaction.parsedData.sourceIp=extractSourceIPAddress(packet);
    transaction.parsedData.destinationIp=extractDestinationIPAddress(packet);

    if (protocol == 6 || protocol == 17) { // TCP or UDP
        transaction.parsedData.sourcePort = (packet.packetData[ipHeaderLength] << 8) | packet.packetData[ipHeaderLength + 1];
        transaction.parsedData.destinationPort = (packet.packetData[ipHeaderLength + 2] << 8) | packet.packetData[ipHeaderLength + 3];
    }
    
    size_t transportHeaderOffset = ipHeaderLength;
    size_t sip_offset = 0;

    if (protocol == 17) { // UDP
        sip_offset = ipHeaderLength + 8; // UDP header is always 8 bytes
    } else if (protocol == 6) { // TCP
        uint8_t tcpHeaderLength = ((packet.packetData[ipHeaderLength + 12] >> 4) & 0xF) * 4; // Data offset field (4 bits) * 4
        sip_offset = ipHeaderLength + tcpHeaderLength;
    }

    std::string sipData = "";
    for (size_t i = sip_offset; i < packet.packetData.size(); ++i) {
        sipData += (char)packet.packetData[i]; // Convert byte to char for readability
    }

    transaction.parsedData.sipMessage.callId = getSipHeader(sipData,"Call-ID");
    transaction.parsedData.sipMessage.fromAddress = getSipHeader(sipData, "From");
    transaction.parsedData.sipMessage.toAddress = getSipHeader(sipData, "To");

    transaction.parsedData.sipMessage.fromExtension = extractExtension(transaction.parsedData.sipMessage.fromAddress);
    transaction.parsedData.sipMessage.toExtension = extractExtension(transaction.parsedData.sipMessage.toAddress); 

    transaction.parsedData.sipMessage.cSeq = getSipHeader(sipData, "CSeq");
    transaction.parsedData.sipMessage.userAgent = getSipHeader(sipData, "User-Agent");

    transaction.parsedData.sdpInfo = extractSDP(sipData);

    //Checking if the incoming packet is a request or response
    bool isRequest=isSipRequest(sipData);
    if(isRequest){
        auto req= std::make_unique<Request>();
        static_cast<TransactionMessage&>(*req)=std::move(transaction);
        req->sipMethod=extractSipMethod(sipData);
        return req;
    }else{
        auto resp= std::make_unique<Response>();
        static_cast<TransactionMessage&>(*resp)=std::move(transaction);
        resp->statusCode=extractStatusCode(sipData);
        resp->status=extractStatus(sipData);
        return resp;
    }
}

void PacketParser::publishToParsedStream(){
    PriorityQueue& priorityQueue = PriorityQueue::getInstance();
    // Read from the priority queue and publish to NATS
    while (!priorityQueue.empty()) {
        std::unique_ptr<TransactionMessage> message = priorityQueue.pop();
        ParsedDataSerializer serializer;
        std::vector<uint8_t> serializedData = serializer.serialize(message);
        ParsedData parsedData=message->parsedData;
        if (auto* req = dynamic_cast<Request*>(message.get())) {
            LOG_DEBUG("SIP Method: {} and Call ID {} parsed Successfully",req->sipMethod,req->parsedData.sipMessage.callId);
            // Handle Request-specific logic
        } else if (auto* res = dynamic_cast<Response*>(message.get())) {
            LOG_DEBUG("SIP Response: {} and Call ID {} parsed Successfully",res->status,res->parsedData.sipMessage.callId);
        }
        LOG_TRACE("SRC IP: {}",parsedData.sourceIp);
        LOG_TRACE("DEST IP: {}",parsedData.destinationIp);
        LOG_TRACE("SRC Port: {}",parsedData.sourcePort);
        LOG_TRACE("DEST Port: {}",parsedData.destinationPort);
        LOG_TRACE("From: {}",parsedData.sipMessage.fromAddress);
        LOG_TRACE("To: {}",parsedData.sipMessage.toAddress);
        LOG_TRACE("CSeq: {}",parsedData.sipMessage.cSeq);

        std::cout << "\nTimestamp: " << parsedData.sipMessage.timestamp << "\n";
        std::cout << "Call-ID: " << parsedData.sipMessage.callId << "\n";
        std::cout << "SRC IP: " << parsedData.sourceIp << "\n";
        std::cout << "DEST IP: " << parsedData.destinationIp << "\n";
        std::cout << "From Extension: " << parsedData.sipMessage.fromExtension << "\n";
        std::cout << "To Extension: " << parsedData.sipMessage.toExtension << "\n";
        std::cout << "SRC Port: " << parsedData.sourcePort << "\n";
        std::cout << "DEST Port: " << parsedData.destinationPort << "\n";
        std::cout << "From: " << parsedData.sipMessage.fromAddress << "\n";
        std::cout << "To: " << parsedData.sipMessage.toAddress << "\n";
        std::cout << "CSeq: " << parsedData.sipMessage.cSeq << "\n";
        std::cout << "UserAgent: " << parsedData.sipMessage.userAgent << "\n\n";

        if (parsedData.sdpInfo) {
            for (const auto& [media, sdp] : *parsedData.sdpInfo) {
                std::cout << "Media: " << media << "\n";
                std::cout << "Port: " << sdp.mediaPort << "\n";
                LOG_TRACE("Media: {}, Port: {}", media, sdp.mediaPort);
                std::cout << "CODECS:\n";
                for (const std::string& codec : sdp.codecs) {
                    std::cout << codec << " ";
                }
                std::cout << "\nNetwork Type: " << sdp.connectionInfo.networkType << "\n";
                std::cout << "Address Type: " << sdp.connectionInfo.addressType << "\n";
                std::cout << "Connection Address: " << sdp.connectionInfo.address << "\n";
                LOG_TRACE("Connection Information Address: {}", sdp.connectionInfo.address);
                std::cout << "Attributes:\n";
                for (const auto& attr : sdp.attributes) {
                    std::cout << attr.name << ": " << attr.value << "\n";
                }
            }
        }

        if (!publisher.publishMessage(ParserConfig::PUB_SUBJECT_NAME, serializedData)) {
            LOG_ERROR("Failed to publish Parsed Data with sequenceNo: {}", message->parsedData.seqNo);
        } else {
            LOG_INFO("Published SIP ParsedData Arrival Time: {}, Packet Size: {} bytes",
                        message->parsedData.sipMessage.timestamp, serializedData.size());
        }
}
}

void PacketParser::run(int threadId,const std::atomic<bool>& running) {
    LOG_INFO("Entered into the run module");
    natsSubscription* threadLocalSub= subscriber.subscriberConsumer(ParserConfig::SUB_SUBJECT_NAME,ParserConfig::CONSUMER_NAME);
    if(!threadLocalSub){
        LOG_ERROR("Error subscribing to the  {} by thread: {}",ParserConfig::CONSUMER_NAME,threadId);
    }else{
        LOG_INFO("Subscribed successfully by thread {} ", threadId);
    }

    PriorityQueue& pq = PriorityQueue::getInstance();

    while (running.load()) {
        natsMsgList list;
        natsStatus status = natsSubscription_Fetch(&list,threadLocalSub, ParserConfig::FETCH_BATCH_SIZE, ParserConfig::FETCH_TIMEOUT_MS, NULL);
        //need to log this error
        if (status == NATS_OK) {
            for (int i = 0; i < list.Count; i++) {
                const uint8_t* data = (const uint8_t*)natsMsg_GetData(list.Msgs[i]);
                size_t length = natsMsg_GetDataLength(list.Msgs[i]);
                PacketDeserializer packetDeserializer;
                SIPPacket packet = packetDeserializer.deserialize(data, length);
                std::unique_ptr<TransactionMessage> message = parseSipPacket(packet);
                if (!message){
                   LOG_ERROR("Nothing was parsed for the packet having sequence Number: {}",packet.sequenceNo);
                    continue;
                } 
                ParsedData parsedData=message->parsedData;

                pq.push(std::move(message)); 

                LOG_INFO("Thread {} Parsed SIP Packet - CSeq {}",threadId,parsedData.sipMessage.cSeq);

                natsMsg_Ack(list.Msgs[i], NULL);
            }
            natsMsgList_Destroy(&list);  
        } else {
            std::cout<<"No messages were fetched...Retrying."<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
    }

    natsSubscription_Destroy(threadLocalSub);;
}



