#include "SIPController.h"
#include "LogMacros.h"

int main(){
    try{
        Logger::init();
        LOG_INFO("SIP Controller Module Started");
        ControllerConfig::loadFromINIFile("../configs/ControllerConfig.ini");
        LOG_INFO("Controller Configuration loaded from INI file");

        NATSJetstream subscriber;
        if(subscriber.connectToNATS(ControllerConfig::natsURL) != 0) return 1;
        if(subscriber.createJetStream() != 0) return 1;
        if(subscriber.createStream(ControllerConfig::SUB_STREAM_NAME,
                                    ControllerConfig::SUB_SUBJECT_NAME) != 0) return 1;
        if(subscriber.createConsumer(ControllerConfig::SUB_STREAM_NAME,
                                    ControllerConfig::SUB_SUBJECT_NAME,
                                    ControllerConfig::CONSUMER_NAME) != 0) return 1;

        NATSJetstream publisher;
        if(publisher.connectToNATS(ControllerConfig::natsURL) != 0) return 1;
        if(publisher.createJetStream() != 0) return 1;
        if(publisher.createStream(ControllerConfig::PUB_STREAM_NAME,
                                ControllerConfig::PUB_SUBJECT_NAME) != 0) return 1;

        
        SIPController controller(subscriber, publisher);
        if(!controller.initialize(ControllerConfig::ruleConfigPath)) return 1;
        controller.run();
        
        subscriber.NATSCleanUp();
        publisher.NATSCleanUp();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
