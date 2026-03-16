#include "MongoDBConfig.h"
#include <INIReader.h>

std::string MongoDBConfig::MONGO_URI;
std::string MongoDBConfig::DB_NAME;
std::string MongoDBConfig::COLLECTION_NAME;
std::string MongoDBConfig::natsURL;
std::string MongoDBConfig::SUB_SUBJECT_NAME;
std::string MongoDBConfig::SUB_STREAM_NAME;
std::string MongoDBConfig::CONSUMER_NAME;
int MongoDBConfig::FETCH_BATCH_SIZE;
int MongoDBConfig::FETCH_TIMEOUT_MS;

bool MongoDBConfig::loadFromINIFile(const std::string& configPath) {
    INIReader reader(configPath);

    if (reader.ParseError() < 0) {
        std::cerr << "Can't load MongoDB config file: " << configPath << std::endl;
        return false;
    }

    MONGO_URI = reader.Get("mongodb", "MONGO_URI", "");
    DB_NAME = reader.Get("mongodb", "DB_NAME", "");
    COLLECTION_NAME = reader.Get("mongodb", "COLLECTION_NAME", "");
    natsURL = reader.Get("nats", "natsURL", "nats://localhost:4222");
    SUB_SUBJECT_NAME = reader.Get("nats", "SUB_SUBJECT_NAME", "session.packet");
    SUB_STREAM_NAME = reader.Get("nats", "SUB_STREAM_NAME", "session_stream");
    CONSUMER_NAME = reader.Get("nats", "CONSUMER_NAME", "session_packet_consumer");
    FETCH_BATCH_SIZE = reader.GetInteger("nats", "FETCH_BATCH_SIZE", 5);
    FETCH_TIMEOUT_MS = reader.GetInteger("nats", "FETCH_TIMEOUT_MS", 1000);


    if (MONGO_URI.empty() || DB_NAME.empty() || COLLECTION_NAME.empty()) {
        std::cerr << "Missing MongoDB configuration fields in INI file." << std::endl;
        return false;
    }

    return true;
}
