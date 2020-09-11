#include "channel/channels_manager.h"

ChannelsManager::ChannelsManager()
{
    BOOST_LOG_TRIVIAL(info) << "create ChannelsManager";
}

bool ChannelsManager::join(subscriber_ptr new_sub, identifier_t new_room_id, Storage::database_ptr db) {
    bool flag_result = true;
    BOOST_LOG_TRIVIAL(info) << "ChannelsManager::join";
    if (auto it=channels.find(new_room_id); it!=channels.end()) {
        BOOST_LOG_TRIVIAL(info) << "channel found";
        clients_in_room.emplace(new_sub->get_client_id(), new_room_id);
        it->second->join(new_sub);
    }
    else {
        BOOST_LOG_TRIVIAL(info) << "channel not found";
        auto [new_it, flag] = channels.emplace(new_room_id, std::make_shared<Channel>(new_room_id, db));
        
        if (!flag) {
            BOOST_LOG_TRIVIAL(error) << "Non create";
            flag_result = false;
            return flag_result;
        }
        // add check
        new_it->second->join(new_sub);

        auto [it2, error] = clients_in_room.emplace(new_sub->get_client_id(), new_room_id);
        if (!error) {
            BOOST_LOG_TRIVIAL(info) << "Non add subsciber client_id="<< it2->first
                                    << " in room_id=" << it2->second;
            flag_result = false;
            return flag_result;
        }
        else {
            BOOST_LOG_TRIVIAL(info) << "New subsciber client_id="<< it2->first
                                    << " in room_id=" << it2->second;
        }
    }

    clientid_to_login.try_emplace(new_sub->get_client_id(), new_sub->get_login());
    new_sub->set_channel(new_room_id);

    return flag_result;
}

void ChannelsManager::send_to_channel(TextSendData data) {
    BOOST_LOG_TRIVIAL(info) << "send to channel_id=" << data.room_id;
    if (auto it=channels.find(data.room_id); it!=channels.end()) {
        it->second->notification(std::move(data));
    }
    else {
        BOOST_LOG_TRIVIAL(info) << "no found room_id=" << data.room_id;
    }
}

void ChannelsManager::leave(identifier_t client_id, identifier_t room_id) {
    BOOST_LOG_TRIVIAL(info) << "ChannelsManager::leave() client_id=" << client_id << ", from room=" << room_id;

    auto it_client = clients_in_room.find(client_id);
    if (it_client == clients_in_room.end()) {
        BOOST_LOG_TRIVIAL(info) << "not found client_id=" << client_id << ", in channels";
        return;
    }
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->leave(client_id);

        clients_in_room.erase(it_client);
        clientid_to_login.erase(client_id);

        BOOST_LOG_TRIVIAL(info) << "client_id=" << client_id
                                << " is leave from room_id=" << room_id;
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "no room room_id=" << room_id;
    }

//    sub->set_busy(false);
}

