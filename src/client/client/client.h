#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <deque>
#include <mutex>
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
    void do_connect(work_buf_req_t&& __buffer);

    void add_msg_to_send(work_buf_req_t &&);

    void start_send_msgs();

    void change_room(int room_id);

    void send_msg_to_server(const std::string& text, int room_id);
    //*****************************************************************************

    void write(const std::string& message);

    void set_login_id(identifier_t id)   { client_id = id;}

    const char* get_login() const { return login; }

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

    std::deque<work_buf_req_t> msg_to_server;

    char login[Block::LoginName];
    char password[Block::Password];
    identifier_t client_id;
    identifier_t room_id = 0;

private:
    void read_input_response();

    void send_login_request(work_buf_req_t && __buffer);

    void read_response_header();

    /**
     * @brief Handle registartion response
     */
    void read_response_data(registr_response_ptr);

    /**
     * @brief Handle authorization response
     * TODO: fix typo autor -> authorization
     */
    void read_response_data(autor_response_ptr);

    /**
     * @brief Handle text message response
     * 
     */
    void read_response_data(text_response_ptr);

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
