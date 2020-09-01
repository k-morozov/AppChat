#include "channel/channels_manager.h"

ChannelsManager::ChannelsManager()
{
    BOOST_LOG_TRIVIAL(info) << "create ChannelsManager";
}

void ChannelsManager::join(subscriber_ptr new_sub, identifier_t room_id, database_ptr db) {
    BOOST_LOG_TRIVIAL(info) << "ChannelsManager::join";
    if (auto it=channels.find(room_id); it!=channels.end()) {
        BOOST_LOG_TRIVIAL(info) << "channel found";
        clients_in_room.emplace(new_sub->get_client_id(), room_id);
        it->second->join(new_sub);
    }
    else {
        BOOST_LOG_TRIVIAL(info) << "channel not found";
        auto [new_it, flag] = channels.emplace(room_id, std::make_shared<Channel>(room_id, db));
        
        if (!flag) {
            BOOST_LOG_TRIVIAL(error) << "Non create";
        }
        // add check
        new_it->second->join(new_sub);

        auto [it2, error] = clients_in_room.emplace(new_sub->get_client_id(), room_id);
        if (!error) {
            BOOST_LOG_TRIVIAL(info) << "Non add subsciber client_id="<< it2->first
                                    << " in room_id=" << it2->second;
        }
        else {
            BOOST_LOG_TRIVIAL(info) << "New subsciber client_id="<< it2->first
                                    << " in room_id=" << it2->second;
        }
    }

    clientid_to_login.try_emplace(new_sub->get_client_id(), new_sub->get_login());
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

void ChannelsManager::leave(subscriber_ptr sub) {
    BOOST_LOG_TRIVIAL(info) << "ChannelsManager::leave()";
    auto it = clients_in_room.find(sub->get_client_id());
    if (it==clients_in_room.end()) {
        BOOST_LOG_TRIVIAL(info) << "not found client_id=" << sub->get_client_id() << ", in channels";
        return;
    }
    auto room_id = it->second;
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->leave(sub);
        clients_in_room.erase(sub->get_client_id());
        clientid_to_login.erase(sub->get_client_id());

        BOOST_LOG_TRIVIAL(info) << "client_id=" << sub->get_client_id()
                                << " is leave from room_id=" << room_id;
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "no room room_id=" << room_id;
    }

    sub->set_busy(false);
}

