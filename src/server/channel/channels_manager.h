#ifndef CHANNELSMANAGER_H
#define CHANNELSMANAGER_H

#include <server/channel/channel.h>

class ChannelsManager
{
public:
    static ChannelsManager& Instance() {
        static ChannelsManager manager;
        return manager;
    }

    void join(subscriber_ptr, identifier_t room_id);
    void send(text_response_ptr);
    void leave(subscriber_ptr);

private:
    ChannelsManager();

    std::unordered_map<identifier_t, iroom_ptr> channels;
    std::unordered_map<identifier_t, identifier_t> clinets_in_room;
};

#endif // CHANNELSMANAGER_H
