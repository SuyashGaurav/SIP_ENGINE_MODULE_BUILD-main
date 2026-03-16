#pragma once

#include "NATSJetstream.h"
#include "TransactionMessage.h"
#include "ControllerConfig.h"
#include "ExtensionRule.h"
#include "CallingPartyRule.h"


class SIPController {
    private:
        NATSJetstream& subscriber;
        NATSJetstream& publisher;
        bool matchRule(const std::unique_ptr<TransactionMessage>& message);
    public:
        SIPController(NATSJetstream& sub, NATSJetstream& pub);
        bool initialize(const std::string& ruleConfigPath);
        void run();
};

