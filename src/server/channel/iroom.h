#ifndef ROOM_H
#define ROOM_H

#include <memory>

#include <server/connection/isubscriber.h>
#include <protocol/protocol.h>


class IRoom
{
public:
    virtual ~IRoom() {}

    virtual void join(subscriber_ptr) = 0;
    virtual void leave(subscriber_ptr) = 0;
    virtual void notification(const Message&) = 0;
};

using iroom_ptr = std::shared_ptr<IRoom>;

#endif // ROOM_H
