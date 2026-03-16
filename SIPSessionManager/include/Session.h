#pragma once
#include <string>
#include "SessionContext.h"
#include "SessionFSM.h"
#include "SDPDescriptor.h"


class Session {
    public:
        std::string callId;
        SessionContext sessionContext;
        SessionFSM fsm;
        SDPDescriptor sdpDescriptor;
};
