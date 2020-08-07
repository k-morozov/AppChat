#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <memory>
#include <deque>

#include "server/connection/isubscriber.h"
#include "server/channel/channels_manager.h"
#include "protocol/protocol.h"
#include "server/log/logger.h"

/**
 * @brief Connection class
 * 
 * @details It serves connected tcp client
 */
class Connection : public ISubscriber, public std::enable_shared_from_this<Connection>
{
public:
    /**
     * @brief Construct a new Connection object
     * 
     * @param _socket Accepted client socket.
     * @param _db
     */
    explicit Connection(boost::asio::ip::tcp::socket&& _socket, database_ptr _db):
        socket(std::move(_socket)),
        db(_db),
        busy(true),
        logger(LOGGER("Connection"))

    {
        LOG4CPLUS_INFO(logger,
                       "new connection from " << socket.remote_endpoint().address().to_string()
                       << ":" << socket.remote_endpoint().port()
                       );
    }

    /**
     * @brief reuse connection for Object Pool
     *
     * @param _socket Accepted client socket.
     */
    void init(boost::asio::ip::tcp::socket&& _socket) {
        packets_to_client.clear();
        client_id = -1;
        login.clear();
        password.clear();

        if (socket.is_open()) {
            socket.close();
        }
        socket = std::move(_socket);

        busy = true;
        LOG4CPLUS_INFO(logger,
                       "init connection from " << socket.remote_endpoint().address().to_string()
                       << ":" << socket.remote_endpoint().port()
                       );
    }

    /**
     * @brief Entry point to handle incoming requests
     */
    virtual void start() override {
        read_request_header();
   }

    /**
     * @brief Send response message to the client
     * 
     * @param response response needs to be sent
     */
    virtual void sendme(text_response_ptr response) override;

    /**
     * @brief Get the client id object
     * 
     * @details Returns current client id
     * 
     * @return identifier_t 
     */
    virtual identifier_t get_client_id() const override {
        return client_id;
    }

    /**
     * @brief Get the login
     * 
     * @details Return client's login
     * 
     * @return const std::string& 
     */
    virtual const std::string& get_login() const override { return login; }

    virtual bool is_busy() const noexcept override { return busy; }
    virtual void set_busy(bool flag = true) noexcept override { busy = flag; }
    /**
      *
      * @todo double close socket?
      *
      * */
    ~Connection() {
        if (socket.is_open()) {
            socket.close();
        }
    }
private:

    boost::asio::ip::tcp::socket socket;
    std::deque<response_ptr> packets_to_client;

    identifier_t client_id;
    std::string login;
    std::string password;

    database_ptr db;
    bool busy;

    log4cplus::Logger logger;

private:
    /**
     * @brief Parse headers of request.
     * 
     * @details It parses requests headers and
     * calls parsing methods for request body when it is neccessary.
     */
    void read_request_header();


    /**
     * @brief Handle registration request.
     */
    void read_request_body(registr_request_ptr);

    /**
     * @brief Handle author request.
     */
    void read_request_body(autor_request_ptr);

    /**
     * @brief Handle incoming message request.
     */
    void read_request_body(text_request_ptr);

    /**
     * @brief Handle room joining request.
     */
    void read_request_body(join_room_request_ptr);

    /**
     * @brief Entry point for sending response.
     * 
     * @details It sends response headers and
     * if successeed call sending response data.
     */
    void send_response_header();

    /**
     * @brief Send response data.
     */
    void send_response_data();

    /**
     * @brief Client id generator.
     * 
     * @details It is quite simple and return incremented integer value every time.
     * 
     * @return identifier_t 
     */
    identifier_t generate_client_id() {
        static identifier_t id = 0;
        return ++id;
    }
};

using connection_ptr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
