#pragma once
#include <memory>
#include <spdlog/spdlog.h>

class Logger {
public:
    static void init();
    static std::shared_ptr<spdlog::logger>& get();

    Logger() = delete;
private:
    static std::shared_ptr<spdlog::logger> logger;
};
