#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "AcceptedState.h"
#include "CallEstablishedState.h"
#include "LogMacros.h"

void AcceptedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestAck*>(&event)) {
        fsm.changeState(std::make_unique<CallEstablishedState>(), context, sdpDescriptor);
    }
}

void AcceptedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "Received Confirmational Response... " << std::endl;
    if(context.sdpInfo.has_value()) {
        sdpDescriptor.calleeConnectionInfoAddress = context.sdpInfo->at("audio").connectionInfo.address;
        sdpDescriptor.calleeAudioPort = context.sdpInfo->at("audio").mediaPort;
        sdpDescriptor.finalCodecs = context.sdpInfo->at("audio").codecs;
    }
    if(sdpDescriptor.callerConnectionInfoAddress!=""){
        std::vector<uint8_t> serializedSession = serializeSession(context.callId, name(),std::nullopt,sdpDescriptor);
        if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
            LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
        }
    }
    else{ //delayed offer
        std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
        if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
            LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
        }
    }
}

void AcceptedState::onExit(SessionContext& context) {
    std::cout << "Leaving Accepted for Call-id..." << context.callId << "\n" << std::endl;
}
