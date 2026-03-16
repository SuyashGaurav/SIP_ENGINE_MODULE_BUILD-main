#include <iostream>
#include "ForcedTerminatedState.h"


void ForcedTerminatedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    // No further events handled in forced termination
}

void ForcedTerminatedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "FSM Forced Termination for call " << context.callId << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }

}

void ForcedTerminatedState::onExit(SessionContext& context) {
}
