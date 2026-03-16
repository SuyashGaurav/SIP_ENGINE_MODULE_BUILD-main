#pragma once
#include <string>
#include <optional>
#include "SessionContext.h"
#include "SessionEvent.h"
#include "SDPDescriptor.h"
#include "NATSJetstream.h"
#include "LogMacros.h"
#include "SessionSerializer.h"


class SessionFSM;
class SessionState{
    public:
        virtual ~SessionState() = default;
        virtual void onEnter(NATSJetstream& publisher, SessionContext& context, SDPDescriptor& sdpDescriptor) = 0;
        virtual void onExit(SessionContext& context) = 0;
        virtual void handleEvent(SessionFSM& fsm, SessionContext& context, const SessionEvent& event, SDPDescriptor& sdpDescriptor) = 0;
        virtual std::string name() const = 0;
};
