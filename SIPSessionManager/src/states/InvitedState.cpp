#include <iostream>
#include "IdleState.h"
#include "SessionEvent.h"
#include "SessionState.h"
#include "InvitedState.h"
#include "AwaitingAuthenticationState.h"
#include "ProgressState.h"
#include "FailedState.h"
#include "LogMacros.h"

void InvitedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const Response1xx*>(&event)) {
        fsm.changeState(std::make_unique<ProgressState>(), context, sdpDescriptor);
    } else if(dynamic_cast<const Response401*>(&event)){
        fsm.changeState(std::make_unique<AwaitingAuthenticationState>(), context, sdpDescriptor);
    } else if(dynamic_cast<const Response4xxTo6xx*>(&event)){
        fsm.changeState(std::make_unique<FailedState>(), context, sdpDescriptor);
    }
}

void InvitedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "Entered the INVITE state for callId: " << context.callId << std::endl;
    if(context.sdpInfo.has_value()){ // early offer
        sdpDescriptor.callerConnectionInfoAddress = context.sdpInfo->at("audio").connectionInfo.address;
        sdpDescriptor.callerAudioPort = context.sdpInfo->at("audio").mediaPort;
    }
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name(), context);
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }

}

void InvitedState::onExit(SessionContext& context){
}
