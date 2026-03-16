#pragma once

#include <string>
#include <iostream>

namespace MongoDBConfig {
    extern std::string MONGO_URI;
    extern std::string DB_NAME;
    extern std::string COLLECTION_NAME;
    extern std::string natsURL;
    extern std::string SUB_SUBJECT_NAME;
    extern std::string SUB_STREAM_NAME;
    extern std::string CONSUMER_NAME;
    extern int FETCH_BATCH_SIZE;
    extern int FETCH_TIMEOUT_MS;

    bool loadFromINIFile(const std::string& configPath);
}
