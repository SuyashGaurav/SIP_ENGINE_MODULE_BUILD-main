#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "SessionContext.h" 
#include "SDPDescriptor.h"
#include <cstring>

void deserializeSession(const uint8_t* data, size_t length,
                        std::string& callId,
                        std::string& currentState,
                        std::optional<SessionContext>& ctxOpt,
                        std::optional<SDPDescriptor>& sdpOpt);
