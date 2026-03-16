#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/collection.hpp>
#include "SessionContext.h"
#include "SDPDescriptor.h"
#include <optional>
#include <iostream>
#include <mutex>

class MongoDatabase {
public:
    static MongoDatabase& getInstance();  // Singleton access

    void initialize(const std::string& mongoURI, const std::string& databaseName, const std::string& collectionName);
    void updateCallState(
        const std::string& callID,
        const std::string& newState,
        const std::optional<SessionContext>& context = std::nullopt,
        const std::optional<SDPDescriptor>& sdpDescriptor = std::nullopt
    );
private:
    MongoDatabase();
    ~MongoDatabase() = default;
    MongoDatabase(const MongoDatabase&) = delete;
    MongoDatabase& operator=(const MongoDatabase&) = delete;

    mongocxx::instance _instance;
    mongocxx::client _client;
    mongocxx::collection _collection;

    bsoncxx::document::value to_bson(const SessionContext& context);
    bsoncxx::document::value to_bson(const SDPDescriptor& sdpDescriptor);
};
