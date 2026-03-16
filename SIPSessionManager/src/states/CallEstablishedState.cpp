#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "CallEstablishedState.h"
#include "SessionModifyingState.h"
#include "ClosingState.h"
#include "LogMacros.h"

void CallEstablishedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestInvite*>(&event)) {
        fsm.changeState(std::make_unique<SessionModifyingState>(), context, sdpDescriptor);
    }
    else if(dynamic_cast<const RequestBye*>(&event)){
        fsm.changeState(std::make_unique<ClosingState>(), context, sdpDescriptor);
    }
}

void CallEstablishedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "Session ongoing for call id " << context.callId << std::endl;
    if(context.sdpInfo.has_value()) { // Delay offer
        sdpDescriptor.callerConnectionInfoAddress = context.sdpInfo->at("audio").connectionInfo.address;
        sdpDescriptor.callerAudioPort = context.sdpInfo->at("audio").mediaPort;
        sdpDescriptor.finalCodecs = context.sdpInfo->at("audio").codecs;
        std::vector<uint8_t> serializedSession = serializeSession(context.callId, name(),std::nullopt,sdpDescriptor);
        if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
            LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
        }

    }
    else{
        std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
        if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
            LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
        }

    }

}

void CallEstablishedState::onExit(SessionContext& context){}
