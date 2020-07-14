#ifndef CHANNELSMANAGER_H
#define CHANNELSMANAGER_H

#include <channel/channel.h>

/**
 * @brief ChannelsManager
 * 
 * @details It is singleton instance that serves channels
 */
class ChannelsManager
{
public:
    static ChannelsManager& Instance() {
        static ChannelsManager manager;
        return manager;
    }

    /**
     * @brief Join user to room by room_id
     * 
     * @param room_id 
     */
    void join(subscriber_ptr, identifier_t room_id);

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
