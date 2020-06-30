#include <server/channel/channel.h>
#include <sstream>


void Channel::join(subscriber_ptr new_subsciber) {
    mutex_subs.lock();
        subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);
    mutex_subs.unlock();

    std::string message("joined to room_id=" + std::to_string(channel_id));

    text_response_ptr response = std::make_shared<TextResponse>(new_subsciber->get_login(), message);
    notification(response);
    std::cout << new_subsciber->get_login() << ": " << message << std::endl;

    for(const auto& response:history_room) {
        new_subsciber->sendme(response);
    }
}

void Channel::leave(subscriber_ptr subsciber) {
    mutex_subs.lock();
        subscribers.erase(subsciber->get_client_id());
    mutex_subs.unlock();

    std::string message(subsciber->get_login() + " leave from roomdid=" + std::to_string(channel_id));

    text_response_ptr response = std::make_shared<TextResponse>("server", message);
    notification(response);
}

void Channel::notification(text_response_ptr response) {
    history_room.push_back(response);
    Database::Instance().save_to_db(response);
    for(auto [id, sub]:subscribers) {
        // @todo add to respose field clientid. now compare string -> will compare int
        if (response->get_login() != sub->get_login()) sub->sendme(response);
    }
}
