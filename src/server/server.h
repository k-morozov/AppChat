#ifndef SERVER_H
#define SERVER_H

#include "session.h"


using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_service& io, boost::asio::ip::tcp::endpoint ep)
        : acc(io, ep)
    {
        do_accept();
    }

private:
    boost::asio::ip::tcp::acceptor acc;
    Chat_room chat_room;

private:
    void do_accept() {
        acc.async_accept([this](const boost::system::error_code& error, tcp::socket sock) {
            std::cout << "new connection" << std::endl;
            if (!error) {
                std::make_shared<Chat_session>(std::move(sock), chat_room)->start();
            }
            do_accept();
        });
    }
};

#endif // SERVER_H
