#pragma once
#include <vector>
#include "INATSClient.h"


class NATSJetstream : public INATSClient {
    private:
        jsConsumerInfo* consumerInfo = nullptr;
        jsStreamInfo* streamInfo = nullptr;
    public:
        natsSubscription* subscription = nullptr;
        int connectToNATS(const std::string& natsURL) override;
        int createJetStream();
        void NATSCleanUp() override;
        int createStream(const std::string& STREAM_NAME, const std::string& SUBJECT_NAME);
        int createConsumer(const std::string& STREAM_NAME, const std::string& SUBJECT_NAME, const std::string& CONSUMER_NAME);
        int subscriberConsumer(const std::string& SUBJECT_NAME, const std::string& CONSUMER_NAME);
        int publishMessage(const std::string& SUBJECT_NAME, const std::vector<uint8_t>& serializedData);
};
