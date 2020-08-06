#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <memory>

#include <connection/connection_manager.h>

using boost::asio::ip::tcp;

/**
 * @brief Async TCP Server.
 * 
 * @details Async TCP Server handling incoming tcp conntection on port 7777.
 */
class Server {
public:
    /**
     * @brief Construct a new Server object.
     * @param argc, argv: first argument - number port
     */
    Server([[maybe_unused]]int argc, [[maybe_unused]]char** argv):
        endpoint(boost::asio::ip::tcp::v4(), (argc>1 ? std::stoi(argv[1]) : SERVER_DEFAULT_PORT)),
        acceptor(io_service, endpoint)
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

    ConnectionManager connect_manager;
private:
    /**
     * @brief Handle new connected clint.
     * 
     * @details It handles client and after that call new acception recursively.
     *
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






