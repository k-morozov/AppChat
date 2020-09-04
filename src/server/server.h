#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <memory>
#include "connection/connection_manager.h"


/**
 * @brief Async TCP Server.
 * @details Async TCP Server handling incoming tcp conntection on port SERVER_DEFAULT_PORT.
 * @todo transform to Load Balancer
 */
class Server {
public:

    /**
     * @brief Construct a new Server object.
     * @param port - number port
     * @param _db - database ptr
     */
    Server(unsigned short port, database_ptr _db):
        endpoint(boost::asio::ip::tcp::v4(), port),
        acceptor(io_service, endpoint),
        // @todo thread_pool
        thread_pool(std::make_shared<boost::asio::thread_pool>(1)),
        db(_db),
        connect_manager(thread_pool, db)
    {
        scan_acception();
    }

    /**
     * @brief Run server.
     */
    void run() {
        boost::asio::signal_set sig(io_service, SIGINT);
        sig.async_wait(std::bind(&Server::signal_handler,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2));

        io_service.run();
    }

private:
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::ip::tcp::acceptor acceptor;
    std::shared_ptr<boost::asio::thread_pool> thread_pool;

    database_ptr db;
    ConnectionManager connect_manager;

    /**
     * @brief Handle new connected clint.
     * @details It handles client and after that call new acception recursively.
     */
    void scan_acception() {
        acceptor.async_accept([this](const boost::system::error_code& error, boost::asio::ip::tcp::socket sock) {
            if (!error) {
                connect_manager.get_connection(std::move(sock))->start();
            }
            scan_acception();
        });
    }

    void signal_handler(const boost::system::error_code&, int) {
        BOOST_LOG_TRIVIAL(fatal) << "called signal_handler()";
        io_service.stop();
        acceptor.close();
        connect_manager.close_all();
    }

};

#endif // SERVER_H






