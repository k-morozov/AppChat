#include <server/channel/channel.h>
#include <sstream>
void Channel::join(subscriber_ptr new_subsciber) {
    mutex_subs.lock();
    subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);
    mutex_subs.unlock();

    std::string message("joined to room_id=" + std::to_string(channel_id));

    notification(new_subsciber->get_login(), message);
    std::cout << new_subsciber->get_login() << ": " << message << std::endl;


//    for(const auto& message:history) {
//        new_subsciber->send(message);
//    }
}

void Channel::leave(subscriber_ptr subsciber) {
    mutex_subs.lock();
    subscribers.erase(subsciber->get_client_id());
    mutex_subs.unlock();

    std::string message(subsciber->get_login() + " leave from Chat_room");

    notification("server", message);
}

void Channel::notification(const std::string& from, const std::string& message) {
    if(!message.empty()) {
        mutex_subs.lock();
        for(auto [id, sub]:subscribers) {
                sub->sendme(from, message);
        }
        mutex_subs.unlock();
    }
}

// @todo update
void Channel::notification(transport_response_ptr response) {
        for(auto [id, sub]:subscribers) {
                sub->sendme(response->get_login(),
                            std::dynamic_pointer_cast<TextResponse>(response)->get_message());
        }
}
