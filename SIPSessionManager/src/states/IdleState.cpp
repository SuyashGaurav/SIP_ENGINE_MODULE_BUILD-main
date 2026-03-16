#include <iostream>
#include "IdleState.h"
#include "SessionEvent.h"
#include "SessionState.h"
#include "InvitedState.h"
#include "LogMacros.h"

void IdleState::handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) {
    if (dynamic_cast<const RequestInvite*>(&event)) {
        fsm.changeState(std::make_unique<InvitedState>(), context, sdpDescriptor);
    }
}

void IdleState::onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor){
    std::cout << "Entering Idle State... Waiting for INVITE" << std::endl;
}

void IdleState::onExit(SessionContext& context){
    std::cout << "Exiting Idle State... " << std::endl;
}
