#pragma once

#include "IRule.h"
#include "RuleConfig.h"


class CallingPartyRule : public IRule {
    public:
        bool match(const std::unique_ptr<TransactionMessage>& message) override {
            if(RuleConfig::callingParties.empty()) return true;
            return RuleConfig::callingParties[message->parsedData.sipMessage.fromExtension];
        }
};
