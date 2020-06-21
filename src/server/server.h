#ifndef SERVER_H
#define SERVER_H

#include <server/session.h>

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_service& io, boost::asio::ip::tcp::endpoint ep)
        : acc(io, ep)
    {
        user_rooms.emplace(Default_room_id, std::make_shared<Chat_room>(Default_room_id));
        do_accept();
    }

private:
    boost::asio::ip::tcp::acceptor acc;
    std::unordered_map<int32_t, room_ptr> user_rooms;

private:
    void do_accept() {
        acc.async_accept([this](const boost::system::error_code& error, tcp::socket sock) {
            if (!error) {
                if (auto it = user_rooms.find(Default_room_id); it!=user_rooms.end()) {
                    std::make_shared<Chat_session>(std::move(sock), it->second)->start();
                }
            }
            else {
                std::cerr << "Accept error" << std::endl;
            }
            do_accept();
        });
    }
};

#endif // SERVER_H
