#include <iostream>
#include "ClosingState.h"
#include "SessionEvent.h"
#include "SessionState.h"
#include "TerminatedState.h"
#include "ForcedTerminatedState.h"
#include "LogMacros.h"

void ClosingState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const Response2xx*>(&event)) {
        fsm.changeState(std::make_unique<TerminatedState>(), context, sdpDescriptor);
    }
    else if (dynamic_cast<const Response4xxTo6xx*>(&event) ||
             dynamic_cast<const Response401*>(&event) ||
             dynamic_cast<const Response408*>(&event) ||
             dynamic_cast<const Response481*>(&event) ||
             dynamic_cast<const Response487*>(&event)) {
        fsm.changeState(std::make_unique<ForcedTerminatedState>(), context, sdpDescriptor);
    }
}

void ClosingState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "FSM In Closing State..." << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }


}

void ClosingState::onExit(SessionContext& context) {
    
}
