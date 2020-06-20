#ifndef SERVER_H
#define SERVER_H

#include <server/session.h>

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_service& io, boost::asio::ip::tcp::endpoint ep)
        : acc(io, ep)
    {
        welcome_room = std::make_shared<Chat_room>();
        do_accept();
    }

private:
    boost::asio::ip::tcp::acceptor acc;
    room_ptr welcome_room;
    std::vector<room_ptr> user_rooms;

private:
    void do_accept() {
        acc.async_accept([this](const boost::system::error_code& error, tcp::socket sock) {
            if (!error) {
                std::make_shared<Chat_session>(std::move(sock), welcome_room)->start();
            }
            do_accept();
        });
    }
};

#endif // SERVER_H
