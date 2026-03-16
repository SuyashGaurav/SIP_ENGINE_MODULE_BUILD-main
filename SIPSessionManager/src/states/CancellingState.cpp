#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "CancellingState.h"
#include "CancelledState.h"
#include "LogMacros.h"

void CancellingState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const Response2xx*>(&event)) {
        // if(context.cSeq ) //check if its for the invite or the cancel method
        fsm.changeState(std::make_unique<CancellingState>(), context, sdpDescriptor);
    }
    else if(dynamic_cast<const Response487*>(&event)){
        fsm.changeState(std::make_unique<CancelledState>(), context, sdpDescriptor);
    }
}

void CancellingState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor){
    std::cout << "FSM In Cancelling State..." << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }


}

void CancellingState::onExit(SessionContext& context){
}
