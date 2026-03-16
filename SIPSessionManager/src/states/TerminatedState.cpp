#include <iostream>
#include "TerminatedState.h"
#include "SessionEvent.h"
#include "SessionState.h"
#include "LogMacros.h"

void TerminatedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    // No further events handled in terminated state
    // This state is the final state for a session
}

void TerminatedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "FSM Terminated for call " << context.callId << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }

}

void TerminatedState::onExit(SessionContext& context){
}
