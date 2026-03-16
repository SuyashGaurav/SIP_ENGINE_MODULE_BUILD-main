#include "MongoDatabase.h"
#include "LogMacros.h"


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;

MongoDatabase::MongoDatabase()
    : _instance(), _client(mongocxx::uri{}) {}

MongoDatabase& MongoDatabase::getInstance() {
    static MongoDatabase instance;
    return instance;
}

void MongoDatabase::initialize(const std::string& mongoURI, const std::string& databaseName, const std::string& collectionName) {
    mongocxx::uri uri{mongoURI};
    _client = mongocxx::client{uri};
    _collection = _client[databaseName][collectionName];
}

bsoncxx::document::value MongoDatabase::to_bson(const SessionContext& context) {
    document doc{};
    doc << "CallerIP" << context.sourceIp
        << "CalleeIP" << context.destinationIp
        << "protocol" << context.protocol
        << "CallerPort" << context.sourcePort
        << "CalleePort" << context.destinationPort
        << "CallerAddress" << context.fromAddress
        << "CalleeAddress" << context.toAddress
        << "userAgent" << context.userAgent
        << "CallerExtension" << context.fromExtension
        << "CalleeExtension" << context.toExtension;

    return doc.extract();
}

bsoncxx::document::value MongoDatabase::to_bson(const SDPDescriptor& sdpDescriptor) {
    using namespace bsoncxx::builder::stream;

    document doc{};
    auto builder = doc
        << "callerConnectionInfoAddress" << sdpDescriptor.callerConnectionInfoAddress
        << "callerAudioPort" << sdpDescriptor.callerAudioPort
        << "calleeConnectionInfoAddress" << sdpDescriptor.calleeConnectionInfoAddress
        << "calleeAudioPort" << sdpDescriptor.calleeAudioPort
        << "finalCodecs" << open_array;

    for (const auto& codec : sdpDescriptor.finalCodecs) {
        builder << codec;
    }
    builder << close_array;

    return doc.extract();
}

void MongoDatabase::updateCallState(
    const std::string& callID,
    const std::string& newState,
    const std::optional<SessionContext>& context,
    const std::optional<SDPDescriptor>& sdpDescriptor
) {
    using namespace bsoncxx::builder::stream;

    document filter{};
    filter << "CallID" << callID;

    document update{};

    // Build the $set document
    document setDoc{};
    setDoc << "CurrentState" << newState;

    if (context.has_value()) {
        setDoc << "SessionContext" << to_bson(context.value()).view();
    }

    update << "$set" << setDoc;

    // Build the $push document
    document pushDoc{};
    pushDoc << "StateHistory" << newState;

    if (sdpDescriptor.has_value()) {
        pushDoc << "SDPDescriptorHistory" << to_bson(sdpDescriptor.value()).view();
    }

    update << "$push" << pushDoc;

    _collection.update_one(
        filter.view(),
        update.view(),
        mongocxx::options::update{}.upsert(true)
    );

    LOG_INFO("Updated call state to newState {} for CallID: {}", newState, callID);
}
