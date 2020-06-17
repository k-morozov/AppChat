#ifndef MANAGER_H
#define MANAGER_H

#include <server/session.h>

class Manager {
public:
    static Manager& Instance() {
        static Manager manager;
        return manager;
    }

    void add_connection(boost::asio::ip::tcp::socket&& sock, Chat_room& chat_room) {
        std::cout << "add connection\n";
        std::make_shared<Chat_session>(std::move(sock), chat_room)->start();
    }
private:
    Manager() = default;
};

#endif // MANAGER_H
