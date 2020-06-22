#include <server/channel/channel.h>

void Channel::join(subscriber_ptr new_subsciber) {
    std::cout << "join to room" << std::endl;
    subscribers.emplace(new_subsciber->get_client_id(), new_subsciber);
    std::cout << "join: OK" << std::endl;
}

void Channel::leave(subscriber_ptr subsciber) {
    subscribers.erase(subsciber->get_client_id());
}

void Channel::notification(const Message& message) {
    for(auto [id, sub]:subscribers) {
        sub->send(message);
    }
}
