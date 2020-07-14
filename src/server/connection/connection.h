#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <memory>
#include <deque>

#include <connection/isubscriber.h>
#include <channel/channels_manager.h>
#include <protocol.h>

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
     */
    explicit Connection(boost::asio::ip::tcp::socket&& _socket):
        socket(std::move(_socket))
    {
        std::cout << "new connection from " << socket.remote_endpoint() .address().to_string()
                  << ":" << socket.remote_endpoint() .port() << std::endl;
    }

    /**
     * @brief Entry point to handle incoming request
     */
    virtual void start() override {
        read_request_header();
    }

    /**
     * @brief Send response to the client
     */
    virtual void sendme(text_response_ptr) override;

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

private:
    boost::asio::ip::tcp::socket socket;
    std::deque<response_ptr> packets_to_client;

    identifier_t client_id;
    std::string login;
    std::string password;

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
