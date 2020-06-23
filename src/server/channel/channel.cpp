#include <server/channel/channel.h>
#include <sstream>
void Channel::join(subscriber_ptr new_subsciber) {

    subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);

    std::string str("joined to room_id=" + std::to_string(channel_id));
    Message notify(str);
    notify.set_login(new_subsciber->get_login());
    notify.set_login_id(new_subsciber->get_client_id());
    notification(notify);
    std::cout << new_subsciber->get_login() << " " << str << std::endl;


    for(const auto& message:history) {
        new_subsciber->send(message);
    }
}

void Channel::leave(subscriber_ptr subsciber) {
    subscribers.erase(subsciber->get_client_id());
    Message mes(subsciber->get_login() + " leave from Chat_room");
    mes.set_login("server");

    notification(mes);
}

void Channel::notification(const Message& message) {
    if(message.get_body_length()) {
        history.push_back(message);
        for(auto [id, sub]:subscribers) {
            if (message.get_login_id() != sub->get_client_id()) {
                sub->send(message);
            }
        }
    }
}
