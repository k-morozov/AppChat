#include <server/channel/channel.h>
#include <sstream>


void Channel::join(subscriber_ptr new_subsciber) {
<<<<<<< HEAD
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
=======
    std::string message(new_subsciber->get_login() + " joined to room_id=" + std::to_string(channel_id));
    std::cout << new_subsciber->get_login() << ": " << message << std::endl;
    text_response_ptr response = std::make_shared<TextResponse>("server", message);
    notification(response);
    for(const auto& response:history_room) {
        new_subsciber->sendme(response);
    }

    mutex_subs.lock();
        subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);
    mutex_subs.unlock();
>>>>>>> dev
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
<<<<<<< HEAD
    history_room.push_back(response);
    for(auto [id, sub]:subscribers) {
        // @todo add to respose field clientid. now compare string -> will compare int
        if (response->get_login() != sub->get_login()) sub->sendme(response);
    }
=======
    for(auto [id, sub]:subscribers) {
        if (sub->get_login() !=response->get_login())
        {
            sub->sendme(response);
        }
    }
    history_room.push_back(response);
>>>>>>> dev
}
