#include <server/channel/channel.h>

void Channel::join(subscriber_ptr new_subsciber) {
    auto [it, flag] = subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);
    std::cout << "join room: " << (flag?"OK":"NO") << std::endl;

    // get history
    for(const auto& message:history) {
        new_subsciber->send(message);
    }
}

void Channel::leave(subscriber_ptr subsciber) {
    subscribers.erase(subsciber->get_client_id());
    Message mes("leave from Chat_room: "
                + std::to_string(static_cast<int>(subsciber->get_client_id())));
    mes.set_login("server");
    notification(mes);
}

void Channel::notification(const Message& message) {
    history.push_back(message);
    for(auto [id, sub]:subscribers) {
        if (message.get_login_id() != sub->get_client_id()) {
            sub->send(message);
        }
    }
}
