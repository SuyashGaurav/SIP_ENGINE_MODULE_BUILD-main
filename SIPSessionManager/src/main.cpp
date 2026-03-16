#include <iostream>
#include "NATSJetstream.h"
#include "SessionManagerConfig.h"
#include "LogMacros.h"
#include "SessionManager.h"

int main(){
    try{
        Logger::init();

        SessionManagerConfig::loadFromINIFile("../configs/SessionManagerConfig.ini");
        LOG_INFO("SessionManager configuration loaded successfully.");

        NATSJetstream subscriber;
        if (subscriber.connectToNATS(SessionManagerConfig::natsURL) != 0) return 1;
        if (subscriber.createJetStream() != 0) return 1;
        if (subscriber.createStream(SessionManagerConfig::SUB_STREAM_NAME,
                                    SessionManagerConfig::SUB_SUBJECT_NAME) != 0) return 1;
        if (subscriber.createConsumer(SessionManagerConfig::SUB_STREAM_NAME,
                                    SessionManagerConfig::SUB_SUBJECT_NAME,
                                    SessionManagerConfig::CONSUMER_NAME) != 0) return 1;

        NATSJetstream publisher;
        if (publisher.connectToNATS(SessionManagerConfig::natsURL) != 0) return 1;
        if (publisher.createJetStream() != 0) return 1;
        if (publisher.createStream(SessionManagerConfig::PUB_STREAM_NAME,
                                SessionManagerConfig::PUB_SUBJECT_NAME) != 0) return 1;

        // Create and run SessionManager
        LOG_INFO("SessionManager module started. Listening for messages...");
        SessionManager sessionManager(subscriber, publisher);
        sessionManager.run();

        subscriber.NATSCleanUp();
        publisher.NATSCleanUp();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
