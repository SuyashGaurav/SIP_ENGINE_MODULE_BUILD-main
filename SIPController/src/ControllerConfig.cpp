#include "ControllerConfig.h"
#include <INIReader.h>

std::string ControllerConfig::SUB_SUBJECT_NAME;
std::string ControllerConfig::SUB_STREAM_NAME;
std::string ControllerConfig::CONSUMER_NAME;
std::string ControllerConfig::PUB_SUBJECT_NAME;
std::string ControllerConfig::PUB_STREAM_NAME;
std::string ControllerConfig::natsURL;
std::string ControllerConfig::ruleConfigPath;
int ControllerConfig::FETCH_BATCH_SIZE;
int ControllerConfig::FETCH_TIMEOUT_MS;

void ControllerConfig::loadFromINIFile(const std::string& configPath) {
    INIReader reader(configPath);

    if (reader.ParseError() < 0) {
        std::cout << "Can't load " << configPath << std::endl;
        return;
    }

    SUB_SUBJECT_NAME  = reader.Get("ControllerConfig", "SUB_SUBJECT_NAME", "parsed.packet");
    SUB_STREAM_NAME   = reader.Get("ControllerConfig", "SUB_STREAM_NAME", "parsed_stream");
    CONSUMER_NAME     = reader.Get("ControllerConfig", "CONSUMER_NAME", "parser_packet_consumer");
    PUB_SUBJECT_NAME  = reader.Get("ControllerConfig", "PUB_SUBJECT_NAME", "controller.packet");
    PUB_STREAM_NAME   = reader.Get("ControllerConfig", "PUB_STREAM_NAME", "controller_stream");
    natsURL           = reader.Get("ControllerConfig", "natsURL", "nats://localhost:4222");
    ruleConfigPath    = reader.Get("ControllerConfig", "ruleConfigPath", "../configs/RuleConfig.ini");
    FETCH_BATCH_SIZE  = reader.GetInteger("ControllerConfig", "FETCH_BATCH_SIZE", 5);
    FETCH_TIMEOUT_MS  = reader.GetInteger("ControllerConfig", "FETCH_TIMEOUT_MS", 1000);
}
