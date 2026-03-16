#pragma once
#include <string>

class SessionEvent {
public:
    virtual ~SessionEvent() = default;
};

class RequestInvite:public SessionEvent {};
class RequestRegister: public SessionEvent {};
class RequestAck:public SessionEvent {};
class RequestUpdate:public SessionEvent {};
class RequestBye:public SessionEvent {};
class RequestRefer:public SessionEvent {};
class RequestCancel:public SessionEvent {};
class Response1xx:public SessionEvent {};
class Response2xx:public SessionEvent {};
class Response3xx:public SessionEvent {};
class Response401:public SessionEvent {};
class Response408:public SessionEvent {};
class Response481:public SessionEvent {};
class Response487:public SessionEvent {};
class Response4xxTo6xx:public SessionEvent {};