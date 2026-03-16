#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "TerminatedState.h"
#include "CancelledState.h"
#include "LogMacros.h"

void CancelledState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestAck*>(&event)) {
        fsm.changeState(std::make_unique<TerminatedState>(), context, sdpDescriptor);
    }
}

void CancelledState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "FSM In Cancelled State..." << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }


}

void CancelledState::onExit(SessionContext& context) {
    
}
