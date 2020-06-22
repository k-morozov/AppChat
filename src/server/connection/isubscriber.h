#ifndef ISUBSCRIBER_H
#define ISUBSCRIBER_H

#include <iostream>
#include <memory>
#include <protocol/protocol.h>

using identifier_t = int32_t;

class ISubscriber
{
public:
    ISubscriber();
    virtual void start() = 0;
    virtual void send(const Message&) = 0;
    virtual identifier_t get_client_id() const = 0;
};

using subscriber_ptr = std::shared_ptr<ISubscriber>;

#endif // ISUBSCRIBER_H
