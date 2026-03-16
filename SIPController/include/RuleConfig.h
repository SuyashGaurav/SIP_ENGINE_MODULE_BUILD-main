#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

namespace RuleConfig {
    extern int extensionRangeMax;
    extern int extensionRangeMin;
    extern std::unordered_map<std::string, bool> excludedExtensions;
    extern std::unordered_map<std::string, bool> callingParties;

    void insertCommaSeparatedValues(const std::string& input, std::unordered_map<std::string, bool>& map);
    bool loadFromINIFile(const std::string& ruleConfigPath);
}
