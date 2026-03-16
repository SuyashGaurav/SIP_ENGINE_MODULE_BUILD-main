#pragma once
#include "SessionState.h"
#include "SessionFSM.h"
#include "SessionContext.h"
#include "SessionEvent.h"

class FailedState : public SessionState {
    void handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) override;
    void onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) override;
    void onExit(SessionContext& context) override;
    std::string name() const override { return "Failed"; }

};
