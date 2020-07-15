#include <channel/channel.h>
#include <sstream>


void Channel::join(subscriber_ptr new_subsciber) {
    std::string message(new_subsciber->get_login() + " joined to room_id=" + std::to_string(channel_id));
    LOG4CPLUS_INFO(logger, new_subsciber->get_login() << ": " << message);
    text_response_ptr response = std::make_shared<TextResponse>("server", message);
    notification(response);
    for(const auto& response:history_room) {
        new_subsciber->sendme(response);
    }

    mutex_subs.lock();
        subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);
    mutex_subs.unlock();
}

void Channel::leave(subscriber_ptr subsciber) {
    mutex_subs.lock();
        subscribers.erase(subsciber->get_client_id());
    mutex_subs.unlock();

    std::string message(subsciber->get_login() + " leave from room_id=" + std::to_string(channel_id));

    text_response_ptr response = std::make_shared<TextResponse>("server", message);
    notification(response);
}

void Channel::notification(text_response_ptr response) {
    for(auto [id, sub]:subscribers) {
        if (sub->get_login() !=response->get_login())
        {
            sub->sendme(response);
        }
    }
    history_room.push_back(response);
}
