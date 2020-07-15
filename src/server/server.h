#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <memory>

#include <connection/connection.h>

using boost::asio::ip::tcp;

class Server {
public:
    Server():
        endpoint(boost::asio::ip::tcp::v4(), 7777),
        acceptor(io_service, endpoint),
        db(std::make_shared<Database>("history.db"))
    {
        scan_acception();
    }

    void run() {
        io_service.run();
    }

private:
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::ip::tcp::acceptor acceptor;
    database_ptr db;

    std::vector<connection_ptr> server_connections;

private:
    void scan_acception() {
        acceptor.async_accept([this](const boost::system::error_code& error, tcp::socket sock) {
            if (!error) {
                auto connect_ptr = std::make_shared<Connection>(std::move(sock), db);
                server_connections.push_back(connect_ptr);
                connect_ptr->start();
            }

            scan_acception();
        });
    }

};

#endif // SERVER_H






