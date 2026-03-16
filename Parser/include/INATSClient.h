#pragma once

#include <iostream>
#include <nats/nats.h>

class INATSClient {
    protected:
        natsStatus status = NATS_OK;
        natsConnection* connection = nullptr;
        jsCtx* jsContext = nullptr;
        jsErrCode jsError;
    public:
        virtual ~INATSClient() = default;
    
        virtual int connectToNATS(const std::string& natsURL) = 0;
        virtual void NATSCleanUp() = 0;
};
