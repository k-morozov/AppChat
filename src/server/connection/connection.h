#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <deque>
#include <mutex>
#include "connection/isubscriber.h"
#include "channel/channels_manager.h"
#include "protocol/protocol.h"
#include "log/logger.h"

#include "protocol/msgfactory.h"
#include <vector>

/**
 * @brief Connection class
 * @details It serves connected tcp client
 */
class Connection : public ISubscriber, public std::enable_shared_from_this<Connection>
{
public:

    /**
     * @brief Construct a new Connection object
     * @param _socket Accepted client socket.
     * @param _db
     */
    explicit Connection(boost::asio::ip::tcp::socket&& _socket, database_ptr _db):
        socket(std::move(_socket)),
        db(_db),
        busy(true)

    {
        BOOST_LOG_TRIVIAL(info) << "new connection from " << socket.remote_endpoint().address().to_string()
                                << ":" << socket.remote_endpoint().port();
    }

    /**
     * @brief reuse connection for Object Pool
     * @param _socket Accepted client socket.
     */
    void reuse(boost::asio::ip::tcp::socket&& _socket) override;

    /**
     * @brief Entry point to handle incoming requests
     */
    virtual void start() override {
        BOOST_LOG_TRIVIAL(info) << "Connection start read_request_header().";
        read_request_header();
   }

    /**
     * @brief Send response message to the client
     * @param response response needs to be sent
     */
    virtual void sendme(text_response_ptr response) override;

    /**
     * @brief Get the client id object
     * @details Returns current client id
     * @return identifier_t 
     */
    virtual identifier_t get_client_id() const override {
        return client_id;
    }

    /**
     * @brief Get the login
     * @details Return client's login
     * @return const std::string& 
     */
    virtual const std::string& get_login() const override { return login; }

    virtual bool is_busy() const noexcept override { return busy; }
    virtual void set_busy(bool flag = true) noexcept override { busy = flag; }

    virtual void free_connection() override;

    ~Connection() {
         free_connection();
    }
private:

    boost::asio::ip::tcp::socket socket;
    std::mutex mtx_sock;
    std::deque<response_ptr> packets_to_client;

    std::vector<uint8_t> __read_buffer;

    identifier_t client_id;
    std::string login;
    std::string password;

    database_ptr db;
    std::atomic<bool> busy;

private:
    /**
     * @brief Parse headers of request.
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
    [[deprecated]]
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
     * @details It is quite simple and return incremented integer value every time.
     * @return identifier_t 
     */
    identifier_t generate_client_id() {
        static identifier_t id = 0;
        return ++id;
    }

    void read_proto_msg(Serialize::Header);
    void read_pb_input_req(boost::system::error_code error, std::size_t);
    void read_pb_reg_req(boost::system::error_code error, std::size_t);
};

using connection_ptr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
