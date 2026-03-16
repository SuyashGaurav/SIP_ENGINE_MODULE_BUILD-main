#include <INIReader.h>
#include "SessionManagerConfig.h"
#include "LogMacros.h"

std::string SessionManagerConfig::SUB_SUBJECT_NAME;
std::string SessionManagerConfig::SUB_STREAM_NAME;
std::string SessionManagerConfig::CONSUMER_NAME;
std::string SessionManagerConfig::PUB_SUBJECT_NAME;
std::string SessionManagerConfig::PUB_STREAM_NAME;
std::string SessionManagerConfig::natsURL;
int SessionManagerConfig::FETCH_BATCH_SIZE;
int SessionManagerConfig::FETCH_TIMEOUT_MS;

void SessionManagerConfig::loadFromINIFile(const std::string& configPath) {
    INIReader reader(configPath);
    
    if(reader.ParseError() < 0) {
        LOG_ERROR("Can't load '{}'", configPath);
        return;
    }

    SUB_SUBJECT_NAME=reader.Get("SessionManager","SUB_SUBJECT_NAME","controller.packet");
    SUB_STREAM_NAME=reader.Get("SessionManager","SUB_STREAM_NAME","controller_stream");
    CONSUMER_NAME=reader.Get("SessionManager","CONSUMER_NAME","controller_packet_consumer");
    PUB_SUBJECT_NAME=reader.Get("SessionManager","PUB_SUBJECT_NAME","session.packet");
    PUB_STREAM_NAME=reader.Get("SessionManager","PUB_STREAM_NAME","session_stream");
    natsURL=reader.Get("SessionManager","natsURL","nats://localhost:4222");
    FETCH_BATCH_SIZE=reader.GetInteger("parser","FETCH_BATCH_SIZE",5);
    FETCH_TIMEOUT_MS=reader.GetInteger("parser","FETCH_TIMEOUT_MS",1000);
}
