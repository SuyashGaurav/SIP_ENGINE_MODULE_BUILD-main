
#include <memory>
#include <string>
#include "SessionFSM.h"
#include "SessionState.h"
#include "SessionContext.h"
#include "SessionEvent.h"
#include "IdleState.h"

SessionFSM::SessionFSM() {
    currentState = std::make_shared<IdleState>();
    
    publisher.connectToNATS(SessionManagerConfig::natsURL);
    publisher.createJetStream();
}

void SessionFSM::handleEvent(SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    currentState->handleEvent(*this, context, event, sdpDescriptor);
}

void SessionFSM::changeState(std::shared_ptr<SessionState> newState, SessionContext& context, SDPDescriptor& sdpDescriptor) {
    if (currentState) {
        currentState->onExit(context);
    }

    currentState = newState;         // Change to the new state

    if (currentState) {
        currentState->onEnter(publisher, context, sdpDescriptor);
    }
}

std::string SessionFSM::currentStateName() const {
    return currentState ? currentState->name() : "NoState";
}

