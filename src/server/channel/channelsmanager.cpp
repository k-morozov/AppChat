#include <server/channel/channelsmanager.h>

ChannelsManager::ChannelsManager()
{
}

void ChannelsManager::join(subscriber_ptr new_sub, identifier_t room_id) {
    if (auto it=channels.find(room_id); it!=channels.end()) {
        clinets_in_room.emplace(new_sub->get_client_id(), room_id);
        it->second->join(new_sub);
    }
    else {
        auto [new_it, flag] = channels.emplace(room_id, std::make_shared<Channel>(room_id));
        if (!flag) {
            std::cout << "Non create" << std::endl;
        }

        auto [it2, error] = clinets_in_room.try_emplace(new_sub->get_client_id(), room_id);
        if (!error) {
            std::cout << "Non add subsciber" << std::endl;
        }

        // add check
        new_it->second->join(new_sub);
    }
}

void ChannelsManager::send(subscriber_ptr from, const Message& message) {
//    auto room_id = message.get_room_id();
    std::cout << "\tfrom clinet_id=" <<  from->get_client_id()
              << ", room_id=" <<  message.get_room_id() << std::endl;
    if (auto it=channels.find(message.get_room_id()); it!=channels.end()) {
        it->second->notification(message);
    }
    else {
        std::cerr << "no room room_id=" << message.get_room_id() << std::endl;
    }
}


void ChannelsManager::leave(subscriber_ptr sub) {
    auto room_id = clinets_in_room[sub->get_client_id()];
    std::cout << "client_id=" << sub->get_client_id()
              << " is leave from " << room_id << std::endl;
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->leave(sub);
    }
    else {
        std::cerr << "no room room_id=" << room_id << std::endl;
    }
}
