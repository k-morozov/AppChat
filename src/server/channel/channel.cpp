#include "channel/channel.h"
#include <sstream>

void Channel::join(subscriber_ptr new_subsciber) {
    BOOST_LOG_TRIVIAL(info) << "Channel::join, count subs=" << subscribers.size();
    std::string message(new_subsciber->get_login() + " joined to room_id=" + std::to_string(channel_id));
    BOOST_LOG_TRIVIAL(info) << message;

    for(const auto& data : history_room) {
        new_subsciber->send_msg_to_client("server", data.text, data.room_id);
    }

    mutex_subs.lock();
        subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);
    for(auto [id, sub]:subscribers) {
        BOOST_LOG_TRIVIAL(info) << "subs: " << sub->get_login() << ", id=" << sub->get_client_id() ;
    }
    mutex_subs.unlock();

    notification({channel_id, "server", message});
}

void Channel::leave(subscriber_ptr subsciber) {
    subsciber->set_busy(false);
    mutex_subs.lock();
        subscribers.erase(subsciber->get_client_id());
    mutex_subs.unlock();

    std::string message(subsciber->get_login() + " leave from room_id=" + std::to_string(channel_id));

    text_response_ptr response = std::make_shared<TextResponse>("server", message);
    notification({channel_id, "server", message});
}

void Channel::notification(TextSendData data) {
    BOOST_LOG_TRIVIAL(info) << "Channel::notification() channel_id=" << channel_id << ", count subs=" << subscribers.size();
    for(auto [id, sub]:subscribers) {
        BOOST_LOG_TRIVIAL(info) << "sub: " << sub->get_login() << " != " << data.login;
//        if (sub->get_login() != data.login)
        {
            sub->send_msg_to_client(data.login, data.text, data.room_id);
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Channel::notification finish" ;
    history_room.emplace_back(std::move(data));
}
