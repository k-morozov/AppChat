#include <server/channel/channels_manager.h>

ChannelsManager::ChannelsManager()
{
}

void ChannelsManager::join(subscriber_ptr new_sub, identifier_t room_id) {
    if (auto it=channels.find(room_id); it!=channels.end()) {
        clinets_in_room.emplace(new_sub->get_client_id(), room_id);
        it->second->join(new_sub);
    }
    else {
        std::cout << "ChannelsManager::join" << std::endl;
        auto [new_it, flag] = channels.emplace(room_id, std::make_shared<Channel>(room_id));
        if (!flag) {
            std::cout << "Non create" << std::endl;
        }
        // add check
        new_it->second->join(new_sub);

        auto [it2, error] = clinets_in_room.emplace(new_sub->get_client_id(), room_id);
        if (!error) {
            std::cout << "Non add subsciber client_id="<< it2->first
                                  << " in room_id=" << it2->second << std::endl;
        }
        else {
            std::cout << "New subsciber client_id="<< it2->first
                      << "in room_id=" << it2->second << std::endl;
        }
    }

    clientid_to_login.try_emplace(new_sub->get_client_id(), new_sub->get_login());
}

void ChannelsManager::send(text_response_ptr response) {
    if (auto it=channels.find(response->get_roomid()); it!=channels.end()) {
        it->second->notification(response);
    }
    else {
        std::cerr << "no room room_id=" << response->get_roomid() << std::endl;
    }
}

void ChannelsManager::leave(subscriber_ptr sub) {
    auto room_id = clinets_in_room[sub->get_client_id()];
    std::cout << "client_id=" << sub->get_client_id()
              << " is leave from " << room_id << std::endl;
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->leave(sub);
        clinets_in_room.erase(sub->get_client_id());
        clientid_to_login.erase(sub->get_client_id());
    }
    else {
        std::cerr << "no room room_id=" << room_id << std::endl;
    }
}

