#pragma once
#include <memory>
#include "IPacketParser.h"
#include "TransactionMessage.h"
#include "NATSJetstream.h"
#include "SIPPacket.h"



class PacketParser:public IPacketParser{
    private:
        std::string getSipHeader(const std::string &sipMessage,const std::string &headerName);
        std::string extractExtension(const std::string &headerValue);
        std::string extractSourceIPAddress(SIPPacket packet); 
        std::string extractDestinationIPAddress(SIPPacket packet); 
        bool isSipRequest(const std::string &sipMessage); 
        std::string extractSipMethod(const std::string &sipMessage); 
        int extractStatusCode(const std::string &status); 
        std::string extractStatus(const std::string &sipMessage);
        std::optional<std::map<std::string,SDP>> extractSDP(const std::string &sipMessage); 
        std::unique_ptr<TransactionMessage> parseSipPacket(SIPPacket packet); 
        
    public: 
        PacketParser(NATSJetstream& pub, NATSJetstream& sub) 
        : IPacketParser(pub,sub) {}
        void publishToParsedStream();
        void run(int threadId,const std::atomic<bool>& running) override;
};