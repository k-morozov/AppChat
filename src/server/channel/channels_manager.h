#ifndef CHANNELSMANAGER_H
#define CHANNELSMANAGER_H

#include "channel.h"
#include "server/log/logger.h"

/**
 * @brief ChannelsManager
 * 
 * @details It is singleton instance that serves channels
 */
class ChannelsManager
{
public:
    /**
     * @brief Getter of singleton instance
     * 
     * @return ChannelsManager& 
     */
    static ChannelsManager& Instance() {
        static ChannelsManager manager;
        return manager;
    }
    /**
     * @brief Join user to room by room_id
     * 
     * @param subscriber room's subscriber
     * 
     * @param room_id room where subscriber joining
     *
     * @param db
     */
    void join(subscriber_ptr subscriber, identifier_t room_id, database_ptr db);

    /**
     * @brief Send message to specific room and specific user
     */
    void send(text_response_ptr);

    /**
     * @brief Leave user from the room
     */
    void leave(subscriber_ptr);

private:
    /**
     * @brief Construct a new Channels Manager
     */
    ChannelsManager();

    std::unordered_map<identifier_t, iroom_ptr> channels;
    std::unordered_map<identifier_t, identifier_t> clients_in_room;
    std::unordered_map<identifier_t, std::string> clientid_to_login;

};

#endif // CHANNELSMANAGER_H
