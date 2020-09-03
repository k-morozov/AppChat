#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <deque>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>
#include "protocol/protocol.h"
#include <QWidget>

#include "protocol/msgfactory.h"

/**
 * @brief Client class
 * 
 * @details Implement communication with server
 */
class Client: public QObject, public std::enable_shared_from_this<Client> {
   Q_OBJECT

public:
    /**
     * @brief Construct a new Client object
     * @param io boost::asio::io_service
     * @param eps 
     * @param request initial request the server
     */
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& _eps)
        : io_service(io), sock(io), eps(_eps)
    {
        std::cout << "create client" << std::endl;
    }

    /**
     * @brief Start connection to the server
     */
    void do_connect(std::vector<uint8_t>&& __buffer);

    void async_read_pb_header();
    void do_read_pb_header(boost::system::error_code error, std::size_t nbytes);

    void async_read_pb_msg(Serialize::Header);

    void add_msg_to_send(std::vector<uint8_t> &&);

    void start_send_msgs();

    void change_room(int room_id);

    void send_msg_to_server(const std::string& text, int room_id);

    void read_pb_text_res(Serialize::Header);

    void set_login(const std::string& new_login) {
        login = new_login;
    }
    //*****************************************************************************

    void write(const std::string& message);

    void set_login_id(identifier_t id)   { client_id = id;}

//    const char* get_login() const { return login; }

    void close_connection();

    ~Client() {
        std::cout << "Destr client" << std::endl;
        close_connection();
    }

private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;
    std::mutex mtx_sock;
    const boost::asio::ip::tcp::resolver::results_type& eps;

    std::vector<uint8_t> __read_buffer;
    std::array<uint8_t, Protocol::SIZE_HEADER> bin_buffer;
    std::deque<std::vector<uint8_t>> msg_to_server;

//    char login[Block::LoginName];
    std::string login;
    char password[Block::Password];
    identifier_t client_id;
    identifier_t room_id = 0;

private:
    void read_input_response(Serialize::Header);
    void do_read_input_response(boost::system::error_code, std::size_t);
    void do_read_join_room_response(boost::system::error_code, std::size_t);
    void do_read_echo_response(boost::system::error_code, std::size_t);

    void send_login_request(std::vector<uint8_t> && __buffer);

    /**
     * @brief Entry point to actually send request
     */
    void send_request_header();

    /**
     * @brief Send request data to server
     * @note Do not call it manually,
     * it must be called from the @link Client::send_request_header "send_reqest_header".
     */
    void send_request_data();


signals:
    /**
     * @brief 
     * @param from sender's login
     * @param text 
     * @param dt date and time of sending the text
     */
    void send_text(const std::string& from, const std::string& text, DateTime dt);

    /**
     * @brief send input code
     */
    void send_input_code(InputCode);
};

#endif // CLIENT_H
