#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "AwaitingAuthenticationState.h"
#include "IdleState.h"
#include "LogMacros.h"

void AwaitingAuthenticationState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestAck*>(&event)) {
        fsm.changeState(std::make_unique<IdleState>(), context, sdpDescriptor);
    }
}

void AwaitingAuthenticationState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "FSM Awaiting Authentication State..." << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }

}

void AwaitingAuthenticationState::onExit(SessionContext& context){
    std::cout << "Exiting Awaiting Authentication State..." << context.callId << "\n" << std::endl;
}
