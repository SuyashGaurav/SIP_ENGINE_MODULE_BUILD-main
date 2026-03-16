#pragma once
#include <atomic>
#include "NATSJetstream.h"

class IPacketParser{
    protected:
    NATSJetstream& publisher;
    NATSJetstream& subscriber;
        
    public:
        IPacketParser(NATSJetstream& pub,NATSJetstream& sub):publisher(pub),subscriber(sub){}
        virtual ~IPacketParser()=default;
        virtual void run(int threadId,const std::atomic<bool>& running)=0;
};