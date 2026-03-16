#include <INIReader.h>
#include "ParserConfig.h"
#include "LogMacros.h"


int ParserConfig::srcIpStartingIndex;
int ParserConfig::destIpStartingIndex;
std::string ParserConfig::SUB_SUBJECT_NAME;
std::string ParserConfig::SUB_STREAM_NAME;
std::string ParserConfig::CONSUMER_NAME;
std::string ParserConfig::PUB_SUBJECT_NAME;
std::string ParserConfig::PUB_STREAM_NAME;
std::string ParserConfig::natsURL;
int ParserConfig::NUM_PARSER_THREADS;
int ParserConfig::FETCH_BATCH_SIZE;
int ParserConfig::FETCH_TIMEOUT_MS;

void ParserConfig::loadFromINIFile(const std::string& configPath) {
    INIReader reader(configPath);
    
    if(reader.ParseError() < 0) {
        LOG_ERROR("Can't load '{}'", configPath);
        return;
    }

    srcIpStartingIndex = reader.GetInteger("parser", "srcIpStartingIndex", 12);
    destIpStartingIndex = reader.GetInteger("parser", "destIpStartingIndex", 16);
    SUB_SUBJECT_NAME=reader.Get("parser","SUB_SUBJECT_NAME","capture.packet");
    SUB_STREAM_NAME=reader.Get("parser","SUB_STREAM_NAME","capture_stream");
    CONSUMER_NAME=reader.Get("parser","CONSUMER_NAME","capture_packet_consumer");
    PUB_SUBJECT_NAME=reader.Get("parser","PUB_SUBJECT_NAME","parsed.packet");
    PUB_STREAM_NAME=reader.Get("parser","PUB_STREAM_NAME","parsed_stream");
    natsURL=reader.Get("parser","natsURL","nats://localhost:4222");
    NUM_PARSER_THREADS=reader.GetInteger("parser","NUM_PARSER_THREADS",4);
    FETCH_BATCH_SIZE=reader.GetInteger("parser","FETCH_BATCH_SIZE",5);
    FETCH_TIMEOUT_MS=reader.GetInteger("parser","FETCH_TIMEOUT_MS",1000);
}

