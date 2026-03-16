#pragma once

#include "SessionContext.h"
#include "SDPDescriptor.h"
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

std::vector<uint8_t> serializeSession(std::string callId,
                                                            std::string currentState,
                                                            std::optional<SessionContext> sessionContext = std::nullopt,
                                                            std::optional<SDPDescriptor> sdpDescriptor = std::nullopt);
