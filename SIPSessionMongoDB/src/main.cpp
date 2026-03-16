#include "MongoDatabase.h"
#include "MongoDBConfig.h"
#include "NATSJetstream.h"
#include "SessionDeserializer.h"
#include "LogMacros.h"

int main() {
    Logger::init();
    if (!MongoDBConfig::loadFromINIFile("../configs/MongoDBConfig.ini")) return 1;
    LOG_INFO("MongoDB configuration loaded successfully");

    MongoDatabase::getInstance().initialize(
        MongoDBConfig::MONGO_URI,
        MongoDBConfig::DB_NAME,
        MongoDBConfig::COLLECTION_NAME
    );
    LOG_INFO("MongoDB initialized successfully");

    NATSJetstream subscriber;
    if (subscriber.connectToNATS(MongoDBConfig::natsURL) != 0) return 1;
    if (subscriber.createJetStream() != 0) return 1;
    if (subscriber.createStream(MongoDBConfig::SUB_STREAM_NAME,
                                MongoDBConfig::SUB_SUBJECT_NAME) != 0) return 1;
    subscriber.subscriberConsumer(MongoDBConfig::SUB_SUBJECT_NAME,MongoDBConfig::CONSUMER_NAME);

    while (true) {
        // Fetch up to 5 messages, wait up to 1 second
        natsMsgList list;
        natsStatus status = natsSubscription_Fetch(&list, subscriber.subscription, 
                                                        MongoDBConfig::FETCH_BATCH_SIZE, 
                                                        MongoDBConfig::FETCH_TIMEOUT_MS, NULL);
        if (status == NATS_OK) {
            for (int i = 0; i < list.Count; i++) {
                const uint8_t* data = (const uint8_t*)natsMsg_GetData(list.Msgs[i]);
                size_t length = natsMsg_GetDataLength(list.Msgs[i]);
                // Deserialize the message
                std::string callID;
                std::string currentState;
                std::optional<SessionContext> sessionContext;
                std::optional<SDPDescriptor> sdpDescriptor;

                deserializeSession(data, length, callID, currentState, sessionContext, sdpDescriptor);

                MongoDatabase::getInstance().updateCallState(callID, currentState,
                                                 sessionContext,
                                                 sdpDescriptor);

                // Acknowledge the message
                natsMsg_Ack(list.Msgs[i], NULL);
            }
            natsMsgList_Destroy(&list);
        }
        else {
            std::cout << "No messages were fetched. Retrying..." << std::endl;
        }
    }
    // Cleanup
    subscriber.NATSCleanUp();

    return 0;
}
