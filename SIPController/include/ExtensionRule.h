#pragma once

#include "IRule.h"
#include "RuleConfig.h"
#include "LogMacros.h"


class ExtensionRule : public IRule {
    public:
        bool match(const std::unique_ptr<TransactionMessage>& message) override {
            std::string extensionFrom = message->parsedData.sipMessage.fromExtension;
            std::string extensionTo = message->parsedData.sipMessage.toExtension;

            if (extensionFrom.empty() || extensionTo.empty()) {
                LOG_WARN("ExtensionRule failed: fromExtension or toExtension is empty");
                return false;
            }

            if (!std::all_of(extensionFrom.begin(), extensionFrom.end(), ::isdigit) ||
                !std::all_of(extensionTo.begin(), extensionTo.end(), ::isdigit)) {
                LOG_WARN("ExtensionRule failed: fromExtension '{}' or toExtension '{}' is not numeric",
                        extensionFrom, extensionTo);
                return false;
            }

            return (std::stoi(extensionFrom) >= RuleConfig::extensionRangeMin && std::stoi(extensionFrom) <= RuleConfig::extensionRangeMax) &&
                   (std::stoi(extensionTo) >= RuleConfig::extensionRangeMin && std::stoi(extensionTo) <= RuleConfig::extensionRangeMax) &&
                   (RuleConfig::excludedExtensions.find(extensionFrom) == RuleConfig::excludedExtensions.end()) &&
                   (RuleConfig::excludedExtensions.find(extensionTo) == RuleConfig::excludedExtensions.end());
        }
};
