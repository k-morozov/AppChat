#ifndef ROOM_H
#define ROOM_H

#include <memory>
#include <string_view>

#include "connection/isubscriber.h"
#include "protocol/protocol.h"

/**
 * @brief Room interface
 * 
 */
class IRoom
{
public:
    
    /**
     * @brief Join a user to channel
     * 
     * @param subcriber sbscriber who is joining this room
     */
    virtual void join(subscriber_ptr subcriber) = 0;

    /**
     * @brief Leave a user from channel
     * 
     * @param subscriber sbscriber who is leaving this room
     */
    virtual void leave(subscriber_ptr subscriber) = 0;
    virtual void leave(identifier_t client_id) = 0;
    /**
     * @brief Notify all room's subscribers about new message
     * @param subscriber
     */
    virtual void notification(TextSendData data) = 0;
    
    /**
     * @brief Get the room id object
     * 
     * @return identifier_t 
     */
    virtual identifier_t get_room_id() const = 0;

    /**
     * @brief Default virtual destructor
     * 
     */
    virtual ~IRoom() = default;
};

using iroom_ptr = std::shared_ptr<IRoom>;

#endif // ROOM_H
