#include <iostream>
#include <thread>
#include <atomic>
#include "NATSJetstream.h"
#include "ParserConfig.h"
#include "LogMacros.h"
#include "PacketParser.h"
std::atomic<bool> running(true);

int main() {
    try{
        Logger::init();
        ParserConfig::loadFromINIFile("../configs/ParserConfig.ini");

        //Create the subscriber for fetching the Packets from Capture Module
        NATSJetstream subscriber;
        if (subscriber.connectToNATS(ParserConfig::natsURL) != 0) return 1;
        if (subscriber.createJetStream() != 0) return 1;
        if(subscriber.createStream(ParserConfig::SUB_STREAM_NAME,
                                    ParserConfig::SUB_SUBJECT_NAME) != 0) return 1;
        if (subscriber.createConsumer(ParserConfig::SUB_STREAM_NAME,
                                    ParserConfig::SUB_SUBJECT_NAME,
                                    ParserConfig::CONSUMER_NAME) != 0) return 1;

        //Create a publisher for publishing Parsed Data from Parser Module
        NATSJetstream publisher;
        if (publisher.connectToNATS(ParserConfig::natsURL) != 0) return 1;
        if (publisher.createJetStream() != 0) return 1;
        if (publisher.createStream(ParserConfig::PUB_STREAM_NAME,
                                ParserConfig::PUB_SUBJECT_NAME) != 0) return 1;

        // Create and run parser
        LOG_INFO("Parser module started.Listening for messages...");
        std::cout << "Parser system is running with " << ParserConfig::NUM_PARSER_THREADS << " threads.\n";
        PacketParser parser(publisher, subscriber);
        
        
        // Launch parser threads
        std::vector<std::thread> workers;
        for (int i = 0; i < ParserConfig::NUM_PARSER_THREADS; ++i) {
            workers.emplace_back([&parser, i]() {
            parser.run(i,running);
    });
        }

        // Background publishing thread
        std::thread publish([&parser]() {
        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            parser.publishToParsedStream();
        }
        });

        // Graceful shutdown (could be signal handler or UI interaction)
        std::cout << "Press Enter to stop...\n";
        std::cin.get();
        running = false;

        // Join all threads
        for (auto &t : workers) t.join();
        publish.join();

        subscriber.NATSCleanUp();
        publisher.NATSCleanUp();

    }catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
