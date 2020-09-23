#include "channel/channels_manager.h"

ChannelsManager::ChannelsManager()
{
    BOOST_LOG_TRIVIAL(info) << "create ChannelsManager";
}

bool ChannelsManager::join(subscriber_ptr new_sub, identifier_t new_room_id, database_ptr db) {
    BOOST_LOG_TRIVIAL(info) << "ChannelsManager::join";
    bool flag_result = true;

    if (auto it_cl = client_in_rooms.find(new_sub->get_client_id()); it_cl == client_in_rooms.end()) {
        auto [nit, flag] = client_in_rooms.emplace(new_sub->get_client_id(), std::deque<identifier_t>());
        if (flag) {
            BOOST_LOG_TRIVIAL(info) << "add channel_id for new client_id";
            nit->second.push_back(new_room_id);
        }
    }
    else {
        BOOST_LOG_TRIVIAL(info) << "add channel_id for old client_id";
        it_cl->second.push_back(new_room_id);
    }



    if (auto it=channels.find(new_room_id); it!=channels.end()) {
        BOOST_LOG_TRIVIAL(info) << "channel found";
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
        new_it->second->join(new_sub);
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

void ChannelsManager::leave_from_all_channels(identifier_t a_client_id) {
    if (auto it = client_in_rooms.find(a_client_id); it != client_in_rooms.end()) {
        for(const auto& m_room_id : it->second) {
            if (auto it_room=channels.find(m_room_id); it_room!=channels.end()) {
                it_room->second->leave(a_client_id);
            }
        }
        client_in_rooms.erase(it);
    }

    clientid_to_login.erase(a_client_id);
}

void ChannelsManager::leave(identifier_t client_id, identifier_t room_id) {
    // @todo need testing -> remove and replcae deque
    BOOST_LOG_TRIVIAL(info) << "ChannelsManager::leave() client_id=" << client_id << ", from room=" << room_id;

    auto it_client = client_in_rooms.find(client_id);
    if (it_client == client_in_rooms.end()) {
        BOOST_LOG_TRIVIAL(info) << "not found client_id=" << client_id << ", in channels";
        return;
    }
    if (auto it=channels.find(room_id); it!=channels.end()) {
        it->second->leave(client_id);

        client_in_rooms.erase(it_client);
        clientid_to_login.erase(client_id);

        BOOST_LOG_TRIVIAL(info) << "client_id=" << client_id
                                << " is leave from room_id=" << room_id;
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "no room room_id=" << room_id;
    }

}

