#include <iostream>
#include "FailedState.h"
#include "SessionEvent.h"
#include "SessionState.h"
#include "FailedState.h"
#include "TerminatedState.h"
#include "LogMacros.h"

void FailedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestAck*>(&event)) {
        fsm.changeState(std::make_unique<TerminatedState>(), context, sdpDescriptor);
    }
}

void FailedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "FSM In Failed State..." << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }


}

void FailedState::onExit(SessionContext& context){
}
