#ifndef ISUBSCRIBER_H
#define ISUBSCRIBER_H

#include <iostream>
#include <memory>
#include <string>
#include <protocol.h>



class ISubscriber
{
public:
    virtual void start() = 0;
    virtual void sendme(text_response_ptr response) = 0;
    virtual identifier_t get_client_id() const = 0;
    virtual const std::string& get_login() const = 0;

    virtual ~ISubscriber() {};
};

using subscriber_ptr = std::shared_ptr<ISubscriber>;

#endif // ISUBSCRIBER_H
