#include <nats/nats.h>
#include <string.h>
#include <iostream>
#include "SessionManager.h"
#include "ParsedPacketDeserializer.h"
#include "SessionManagerConfig.h"
#include "TransactionMessage.h"
#include "Request.h"
#include "Response.h"
#include "LogMacros.h"
#include "SessionSerializer.h"


void SessionManager::processParsedPacket(std::unique_ptr<TransactionMessage>& message){
    ParsedData parsedData=message->parsedData;
    Session& session=sessions[parsedData.sipMessage.callId];
    if (auto* req = dynamic_cast<Request*>(message.get())) {

        if(req->sipMethod == "INVITE"){
            session.fsm.handleEvent(session.sessionContext, RequestInvite(), session.sdpDescriptor);
        }
        else if(req->sipMethod=="ACK"){
            session.fsm.handleEvent(session.sessionContext, RequestAck(), session.sdpDescriptor);
        }
        else if(req->sipMethod=="CANCEL"){
            session.fsm.handleEvent(session.sessionContext, RequestCancel(), session.sdpDescriptor);
        }
        else if(req->sipMethod=="BYE"){
            session.fsm.handleEvent(session.sessionContext, RequestBye(), session.sdpDescriptor);
        }
        else if(req->sipMethod=="UPDATE"){
            session.fsm.handleEvent(session.sessionContext, RequestUpdate(), session.sdpDescriptor);
        }
        else if(req->sipMethod=="REFER"){
            session.fsm.handleEvent(session.sessionContext, RequestRefer(), session.sdpDescriptor);
        }
    } else if (auto* res = dynamic_cast<Response*>(message.get())) {
        if(res->statusCode>=100 && res->statusCode<200){
            session.fsm.handleEvent(session.sessionContext, Response1xx(), session.sdpDescriptor);
        }
        else if(res->statusCode>=200 && res->statusCode<300){
            session.fsm.handleEvent(session.sessionContext, Response2xx(), session.sdpDescriptor);
        }
        else if(res->statusCode>=300 && res->statusCode<400){
            session.fsm.handleEvent(session.sessionContext, Response3xx(), session.sdpDescriptor);
        }
        else if(res->statusCode==401){
            session.fsm.handleEvent(session.sessionContext, Response401(), session.sdpDescriptor);
        }
        else if(res->statusCode==408){
            session.fsm.handleEvent(session.sessionContext, Response408(), session.sdpDescriptor);
        }
        else if(res->statusCode==481){
            session.fsm.handleEvent(session.sessionContext, Response481(), session.sdpDescriptor);
        }
        else if(res->statusCode==487){
            session.fsm.handleEvent(session.sessionContext, Response487(), session.sdpDescriptor);
        }
        else {
            session.fsm.handleEvent(session.sessionContext, Response4xxTo6xx(), session.sdpDescriptor);
        }
    }
    else {
        LOG_ERROR("Invalid Packet");
    }
}

void SessionManager::updateSessionContextFromMessage(ParsedData data){
    std::string callId=data.sipMessage.callId;
    sessions[callId].sdpDescriptor.callid=callId;
    sessions[callId].sessionContext.callId=callId;
    sessions[callId].sessionContext.timestamp=data.sipMessage.timestamp;
    sessions[callId].sessionContext.sourceIp=data.sourceIp;
    sessions[callId].sessionContext.destinationIp=data.destinationIp;
    sessions[callId].sessionContext.protocol=data.protocol;
    sessions[callId].sessionContext.sourcePort=data.sourcePort;
    sessions[callId].sessionContext.destinationPort=data.destinationPort;
    sessions[callId].sessionContext.cSeq=data.sipMessage.cSeq;
    sessions[callId].sessionContext.fromAddress=data.sipMessage.fromAddress;
    sessions[callId].sessionContext.toAddress=data.sipMessage.toAddress;
    sessions[callId].sessionContext.fromExtension=data.sipMessage.fromExtension;
    sessions[callId].sessionContext.toExtension=data.sipMessage.toExtension;
    sessions[callId].sessionContext.userAgent=data.sipMessage.userAgent;
    if(data.sdpInfo.has_value())  {
        sessions[callId].sessionContext.sdpInfo = data.sdpInfo;
    } else {
        sessions[callId].sessionContext.sdpInfo = std::nullopt;
    }
}

void SessionManager::run(){
    ParsedPacketDeserializer deserializer;
    subscriber.subscriberConsumer(SessionManagerConfig::SUB_SUBJECT_NAME,SessionManagerConfig::CONSUMER_NAME);
    LOG_INFO("Subscribed to durable consumer {}", SessionManagerConfig::CONSUMER_NAME);
    while (true) {
        // Fetch up to 5 messages, wait up to 1 second
        natsMsgList list;
        natsStatus status = natsSubscription_Fetch(&list, subscriber.subscription, 
                                                        SessionManagerConfig::FETCH_BATCH_SIZE, 
                                                        SessionManagerConfig::FETCH_TIMEOUT_MS, NULL);
        if (status == NATS_OK) {
            for (int i = 0; i < list.Count; i++) {
                const uint8_t* data = (const uint8_t*)natsMsg_GetData(list.Msgs[i]);
                size_t length = natsMsg_GetDataLength(list.Msgs[i]);
                std::unique_ptr<TransactionMessage> message = deserializer.deserializePacket(data, length);
                std::string callId=message->parsedData.sipMessage.callId;
                auto* req = dynamic_cast<Request*>(message.get());
                if(!(sessionExists(callId)) && req && req->sipMethod == "INVITE") {
                    std::cout << "Creating New FSM for call ID: " << callId<<std::endl;
                    Session newSession;
                    newSession.callId = callId;
                    newSession.sessionContext = SessionContext();
                    newSession.fsm = SessionFSM();
                    newSession.sdpDescriptor = SDPDescriptor();
                    std::vector<uint8_t> serializedSession = serializeSession(newSession.callId, newSession.fsm.currentStateName());
                    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
                        LOG_INFO("Published new session state context for call ID: {}, {}", callId, newSession.fsm.currentStateName());
                    }
                    sessions[callId] = std::move(newSession);
                }
                updateSessionContextFromMessage(message->parsedData);
                processParsedPacket(message);

                // Acknowledge the message
                natsMsg_Ack(list.Msgs[i], NULL);
            }
            natsMsgList_Destroy(&list);
        }
        else {
            std::cout << "No messages were fetched. Retrying..." << std::endl;
        }
    }
}
