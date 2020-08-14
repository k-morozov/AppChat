#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <memory>

#include "connection/connection_manager.h"

using boost::asio::ip::tcp;

/**
 * @brief Async TCP Server.
 * @details Async TCP Server handling incoming tcp conntection on port SERVER_DEFAULT_PORT.
 */
class Server {
public:
    /**
     * @brief Construct a new Server object.
     * @param port - number port
     * @param _db - database ptr
     */
    Server(int32_t port, database_ptr _db):
        endpoint(boost::asio::ip::tcp::v4(), port),
        acceptor(io_service, endpoint),
        db(_db),
        connect_manager(db)
    {
        scan_acception();
    }

    /**
     * @brief Run server.
     */
    void run() {
        io_service.run();
    }

private:
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::ip::tcp::acceptor acceptor;
    database_ptr db;
    ConnectionManager connect_manager;

    /**
     * @brief Handle new connected clint.
     * @details It handles client and after that call new acception recursively.
     */
    void scan_acception() {
        acceptor.async_accept([this](const boost::system::error_code& error, tcp::socket sock) {
            if (!error) {
                connect_manager.get_connection(std::move(sock))->start();
            }
            scan_acception();
        });
    }

};

#endif // SERVER_H






