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
    void join(subscriber_ptr, identifier_t room_id = 0);
    void send(identifier_t room_id, const Message&);
private:
    ChannelsManager();

    std::unordered_map<identifier_t, iroom_ptr> channels;

};

#endif // CHANNELSMANAGER_H
