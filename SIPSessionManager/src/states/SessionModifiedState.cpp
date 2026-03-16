#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "CallEstablishedState.h"
#include "SessionModifiedState.h"
#include "LogMacros.h"

void SessionModifiedState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestAck*>(&event)) {
        fsm.changeState(std::make_unique<CallEstablishedState>(), context, sdpDescriptor);
    }
}

void SessionModifiedState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "Session Modified" << context.callId << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }


}

void SessionModifiedState::onExit(SessionContext& context){
}
