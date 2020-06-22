#include <server/channel/channelsmanager.h>

ChannelsManager::ChannelsManager()
{
     channels.emplace(0, std::make_shared<Channel>());
}

void ChannelsManager::join(subscriber_ptr new_sub, identifier_t room_id) {
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->join(new_sub);
    }
    else {
        std::cerr << "no room room_id=" << room_id << std::endl;
    }
}

void ChannelsManager::send(identifier_t room_id, const Message& message) {
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->notification(message);
    }
    else {
        std::cerr << "no room room_id=" << room_id << std::endl;
    }
}
