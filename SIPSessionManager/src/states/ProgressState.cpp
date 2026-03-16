#include <iostream>
#include "SessionEvent.h"
#include "SessionState.h"
#include "ProgressState.h"
#include "FailedState.h"
#include "AcceptedState.h"
#include "LogMacros.h"

void ProgressState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const Response1xx*>(&event)) {
        fsm.changeState(std::make_unique<ProgressState>(), context, sdpDescriptor);
    } else if(dynamic_cast<const Response4xxTo6xx*>(&event)){
        fsm.changeState(std::make_unique<FailedState>(), context, sdpDescriptor);
    } else if(dynamic_cast<const Response2xx*>(&event)) {
        fsm.changeState(std::make_unique<AcceptedState>(), context, sdpDescriptor);
    }
}

void ProgressState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    std::cout << "Received Provisional Response... " << std::endl;
    std::vector<uint8_t> serializedSession = serializeSession(context.callId, name());
    if(publisher.publishMessage(SessionManagerConfig::PUB_SUBJECT_NAME, serializedSession)) {
        LOG_INFO("Published new session state context for call ID: {}, {}", context.callId, name());
    }
}

void ProgressState::onExit(SessionContext& context) {
}
