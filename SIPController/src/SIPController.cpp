#include "SIPController.h"
#include "ParsedPacketSerializer.h"
#include "ParsedPacketDeserializer.h"
#include "LogMacros.h"

SIPController::SIPController(NATSJetstream& sub, NATSJetstream& pub)
    : subscriber(sub), publisher(pub) {}

bool SIPController::initialize(const std::string& ruleConfigPath) {
    bool success = RuleConfig::loadFromINIFile(ruleConfigPath);
    if (success) {
        LOG_INFO("Rule configuration loaded from: {}", ruleConfigPath);
    } else {
        LOG_ERROR("Failed to load rule configuration from: {}", ruleConfigPath);
    }
    return success;
}

bool SIPController::matchRule(const std::unique_ptr<TransactionMessage>& message) {
    ExtensionRule extensionRule;
    CallingPartyRule callingPartyRule;

    if (!extensionRule.match(message)) {
        LOG_TRACE("[Rule Failed] ExtensionRule | From: {}, To: {}",
                 message->parsedData.sipMessage.fromExtension,
                 message->parsedData.sipMessage.toExtension);
        return false;
    }
    if (!callingPartyRule.match(message)) {
        LOG_TRACE("[Rule Failed] CallingPartyRule | Calling: {}",
                 message->parsedData.sipMessage.fromExtension);
        return false;
    }
    LOG_TRACE("Both ExtensionRule and CallingPartyRule matched for Parsed Message with seqNo: {}", message->parsedData.seqNo);

    return true;
}

void SIPController::run() {
    ParsedPacketSerializer serializer;
    ParsedPacketDeserializer deserializer;

    subscriber.subscriberConsumer(ControllerConfig::SUB_SUBJECT_NAME, ControllerConfig::CONSUMER_NAME);
    LOG_INFO("Subscribed to durable consumer *{}* (bound to stream *{}*, filtering subject *{}*)", ControllerConfig::CONSUMER_NAME, ControllerConfig::SUB_STREAM_NAME, ControllerConfig::SUB_SUBJECT_NAME);


    while (true) {
        // Fetch up to 5 messages, wait up to 1 second
        natsMsgList list;
        natsStatus status = natsSubscription_Fetch(&list, subscriber.subscription, 
                                                          ControllerConfig::FETCH_BATCH_SIZE, 
                                                          ControllerConfig::FETCH_TIMEOUT_MS, NULL);
        if (status == NATS_OK) {
            for (int i = 0; i < list.Count; i++) {
                const uint8_t* data = (const uint8_t*)natsMsg_GetData(list.Msgs[i]);
                size_t length = natsMsg_GetDataLength(list.Msgs[i]);
                std::unique_ptr<TransactionMessage> message = deserializer.deserializePacket(data, length);

                if(matchRule(message)) {
                    std::vector<uint8_t> serializedData = serializer.serializePacket(message);
                    if (!publisher.publishMessage(ControllerConfig::PUB_SUBJECT_NAME, serializedData)) {
                        LOG_ERROR("Failed to publish Parsed Data | Sequence No: {}", message->parsedData.seqNo);
                        continue;
                    } else {
                        LOG_INFO("Published SIP ParsedData | Sequence No: {}, Packet Size: {} bytes",
                                 message->parsedData.seqNo, serializedData.size());
                    }
                } else {
                    LOG_DEBUG("Message with sequenceNo: {} did not match rules and will be dropped",
                              message->parsedData.seqNo);
                }
 
                // Acknowledge the message
                natsMsg_Ack(list.Msgs[i], NULL);
            }
            natsMsgList_Destroy(&list);
        } else {
            std::cout << "No new messages available from Parser. Retrying..." << std::endl;
        }
    }
}
