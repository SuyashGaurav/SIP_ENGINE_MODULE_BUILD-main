#pragma once
#include <map>
#include <string>
#include <memory>
#include "Session.h"
#include "NATSJetstream.h"
#include "TransactionMessage.h"
#include "ParsedData.h"


class SessionManager{
    protected:
        std::map<std::string,Session> sessions;
        NATSJetstream& subscriber;
        NATSJetstream& publisher;
    public:
        SessionManager(NATSJetstream& sub, NATSJetstream& pub): subscriber(sub), publisher(pub){}
        void processParsedPacket(std::unique_ptr<TransactionMessage>& message);
        void run();
        void updateSessionContextFromMessage(ParsedData data);
        bool sessionExists(const std::string& callId) const {
        return sessions.find(callId) != sessions.end();}
};
