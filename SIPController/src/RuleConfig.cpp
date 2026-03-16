#include "RuleConfig.h"
#include <INIReader.h>
#include <sstream>

int RuleConfig::extensionRangeMax = 0;
int RuleConfig::extensionRangeMin = 0;
std::unordered_map<std::string, bool> RuleConfig::excludedExtensions;
std::unordered_map<std::string, bool> RuleConfig::callingParties;

// Helper to split a comma-separated string and store in map
void RuleConfig::insertCommaSeparatedValues(const std::string& input, std::unordered_map<std::string, bool>& map) {
    std::istringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            map[token] = true;
        }
    }
}

bool RuleConfig::loadFromINIFile(const std::string& ruleConfigPath) {
    INIReader reader(ruleConfigPath);

    if (reader.ParseError() < 0) {
        std::cerr << "Can't load rule config: " << ruleConfigPath << std::endl;
        return false;
    }

    insertCommaSeparatedValues(reader.Get("rules", "ExcludedExtensions", ""), excludedExtensions);
    insertCommaSeparatedValues(reader.Get("rules", "CallingParty", ""), callingParties);
    
    std::string rangeString = reader.Get("rules", "ExtensionRange", "");
    size_t dashPosition = rangeString.find('-');
    if (dashPosition != std::string::npos) {
        try {
            extensionRangeMin = std::stoi(rangeString.substr(0, dashPosition));
            extensionRangeMax = std::stoi(rangeString.substr(dashPosition + 1));
        } catch (const std::exception& e) {
            std::cerr << "Invalid ExtensionRange format: " << rangeString << std::endl;
            return false;
        }
    } else {
        std::cerr << "Missing '-' in ExtensionRange: " << rangeString << std::endl;
        return false;
    }

    return true;
}

