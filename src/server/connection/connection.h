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
#include <cassert>

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
        async_read_pb_header();
   }

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
    virtual void set_channel(identifier_t new_id_room) override { room_id = new_id_room; }
    virtual bool is_busy() const noexcept override { return busy; }
    virtual void set_busy(bool flag = true) noexcept override { busy = flag; }

    virtual void free_connection() override;

    ~Connection() {
         free_connection();
    }

private:
    boost::asio::ip::tcp::socket socket;
    std::mutex mtx_sock;
//    std::deque<response_ptr> packets_to_client;

    std::deque<Protocol::work_buf_req_t> msg_to_client;
    std::deque<std::vector<uint8_t>> bin_buf_to_client;

    identifier_t client_id;
    identifier_t room_id = 0;
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
    void async_read_pb_header();
    void do_read_pb_header(boost::system::error_code, std::size_t);

    std::array<uint8_t, Protocol::SIZE_HEADER> buffer_header;
    std::vector<uint8_t> buffer_msg;

    identifier_t generate_client_id() {
        static identifier_t id = 0;
        return ++id;
    }

    void async_read_proto_msg(Serialize::Header) override;
    void do_read_pb_input_req(boost::system::error_code, std::size_t) override;
    void do_write_input_req(boost::system::error_code, std::size_t);

    void read_pb_reg_req(boost::system::error_code, std::size_t) override;
    void read_pb_join_room_req(boost::system::error_code, std::size_t) override;
    void read_pb_text_req(boost::system::error_code, std::size_t) override;
    void send_msg_to_client(const std::string&,const std::string&, int) override;
//    void start_send_msgs() override;
    void add_bin_buf_to_send(std::vector<uint8_t>&&);
    void start_send_bin_buffers();
};

using connection_ptr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
