#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "SessionModifyingState.h"
#include "SessionModifiedState.h"
#include "TerminatedState.h"
#include "SessionModificationRefusedState.h"
#include "LogMacros.h"

void SessionModifyingState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const Response1xx*>(&event)) {
        fsm.changeState(std::make_unique<SessionModifyingState>(), context, sdpDescriptor);
    }
    else if(dynamic_cast<const Response2xx*>(&event)){
        fsm.changeState(std::make_unique<SessionModifiedState>(), context, sdpDescriptor);

    }
    else if((dynamic_cast<const Response481*>(&event))||(dynamic_cast<const Response408*>(&event))){
        fsm.changeState(std::make_unique<TerminatedState>(), context, sdpDescriptor);
    }
    else if(dynamic_cast<const Response4xxTo6xx*>(&event)){
        fsm.changeState(std::make_unique<SessionModificationRefusedState>(), context, sdpDescriptor);
    }
}

void SessionModifyingState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor){
    std::cout << "Session Modifying for call id " << context.callId << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }
}

void SessionModifyingState::onExit(SessionContext& context){
}
