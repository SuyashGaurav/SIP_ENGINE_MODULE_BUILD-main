#include "NATSJetstream.h"
#include "LogMacros.h"


int NATSJetstream::connectToNATS(const std::string& natsURL) {
    status = natsConnection_ConnectTo(&connection, natsURL.c_str());
    if (status != NATS_OK) {
        LOG_ERROR("Error connecting to NATS: {}", natsStatus_GetText(status));
        return 1;
    }
    LOG_INFO("Connected to NATS server at {}", natsURL);
    return 0;
}

int NATSJetstream::createJetStream() {
    status = natsConnection_JetStream(&jsContext, connection, nullptr);
    if (status != NATS_OK) {
        LOG_ERROR("Error creating JetStream context: {}", natsStatus_GetText(status));
        natsConnection_Destroy(connection);
        return 1;
    }
    LOG_INFO("JetStream context created successfully.");
    return 0;
}

void NATSJetstream::NATSCleanUp() {
    jsStreamInfo_Destroy(streamInfo);
    jsCtx_Destroy(jsContext);
    natsSubscription_Destroy(subscription);
    natsConnection_Destroy(connection);
    nats_Close();
    LOG_INFO("NATS resources cleaned up.");
}

int NATSJetstream::createStream(const std::string& STREAM_NAME, const std::string& SUBJECT_NAME) {
    jsStreamConfig cfg;
    jsStreamConfig_Init(&cfg);

    cfg.Name = STREAM_NAME.c_str();
    const char* subjects[] = { SUBJECT_NAME.c_str() };
    cfg.Subjects = subjects;
    cfg.SubjectsLen = 1;
    cfg.Storage = js_MemoryStorage;
    // cfg.Retention = js_WorkQueuePolicy;
    cfg.Retention = js_LimitsPolicy;

    status = js_GetStreamInfo(&streamInfo, jsContext, STREAM_NAME.c_str(), nullptr, &jsError);
    if (status != NATS_OK) {
        LOG_INFO("Stream does not exist, creating new stream *{}*", STREAM_NAME);
        status = js_AddStream(&streamInfo, jsContext, &cfg, nullptr, &jsError);
        if (status != NATS_OK) {
            LOG_ERROR("Error adding stream: {}  - JetStream error code enum: {}", 
                natsStatus_GetText(status), static_cast<int>(jsError));
            return 1;
        }
    } else {
        LOG_INFO("Stream *{}* already exists.", STREAM_NAME);
    }
    return 0;
}

int NATSJetstream::createConsumer(const std::string& STREAM_NAME, const std::string& SUBJECT_NAME, const std::string& CONSUMER_NAME) {
    jsConsumerConfig consumerCfg;
    jsConsumerConfig_Init(&consumerCfg);

    consumerCfg.Durable = CONSUMER_NAME.c_str();
    consumerCfg.FilterSubject = SUBJECT_NAME.c_str();
    consumerCfg.AckPolicy = js_AckExplicit;
    consumerCfg.DeliverPolicy = js_DeliverAll;

    status = js_AddConsumer(&consumerInfo, jsContext, STREAM_NAME.c_str(), &consumerCfg, nullptr, &jsError);
    if (status != NATS_OK) {
        LOG_ERROR("Error creating consumer: {}  - JetStream error code enum: {}", 
            natsStatus_GetText(status), static_cast<int>(jsError));
        return 1;
    }
    LOG_INFO("Consumer *{}* created successfully.", CONSUMER_NAME);
    return 0;
}

int NATSJetstream::subscriberConsumer(const std::string& SUBJECT_NAME, const std::string& CONSUMER_NAME) {
    jsSubOptions subOpts;
    jsSubOptions_Init(&subOpts);

    status = js_PullSubscribe(&subscription, jsContext, SUBJECT_NAME.c_str(), CONSUMER_NAME.c_str(), nullptr, &subOpts, &jsError);
    if (status != NATS_OK) {
        LOG_ERROR("Error subscribing to consumer: {}  - JetStream error code: {}", 
            natsStatus_GetText(status), static_cast<int>(jsError));
        return 1;
    }
    return 0;
}

int NATSJetstream::publishMessage(const std::string& SUBJECT_NAME, const std::vector<uint8_t>& serializedData) {
    jsPubAck* pubAck = nullptr;

    status = js_Publish(&pubAck, jsContext, SUBJECT_NAME.c_str(), serializedData.data(), serializedData.size(), nullptr, &jsError);
    if (status != NATS_OK) {
        LOG_ERROR("Error publishing message: {}  - JetStream error code enum: {}", 
            natsStatus_GetText(status), static_cast<int>(jsError));
        return 0;
    }

    jsPubAck_Destroy(pubAck);
    return 1;
}
