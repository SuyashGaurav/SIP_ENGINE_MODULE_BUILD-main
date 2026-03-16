#pragma once
#include <memory>
#include <optional>
#include "SessionState.h"
#include "SessionContext.h"
#include "SessionEvent.h"
#include "NATSJetstream.h"
#include "SessionManagerConfig.h"
#include "SDPDescriptor.h"

class SessionFSM {
private:
    std::shared_ptr<SessionState> currentState;
    NATSJetstream publisher;
public:
    SessionFSM();
    void handleEvent(SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor);
    void changeState(std::shared_ptr<SessionState> newState, SessionContext& context, SDPDescriptor& sdpDescriptor);
    std::string currentStateName() const;
};
