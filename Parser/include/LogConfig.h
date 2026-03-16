#pragma once
#include <string>
#include <spdlog/spdlog.h>

namespace LogConfig {
    extern std::string loggerName;
    extern std::string logFilePath;
    extern size_t maxFileSize;
    extern int maxFiles;
    extern std::string logPattern;
    extern spdlog::level::level_enum logLevel;
    extern int FlushIntervalSeconds;

    void loadFromINIFile(const std::string& configPath);
}
