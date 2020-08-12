#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <deque>
#include <boost/asio.hpp>
#include "protocol/protocol.h"
#include <QWidget>

/**
 * @brief Client class
 * 
 * @details Implement communication with server
 */
class Client: public QObject {
   Q_OBJECT

public:
    /**
     * @brief Construct a new Client object
     * @param io boost::asio::io_service
     * @param eps 
     * @param request initial request the server
     */
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps,
           input_request_ptr request)
        : io_service(io), sock(io), eps(eps)
    {
        std::cout << "ctor client" << std::endl;
        do_connect(eps, request);
    }

    /**
     * @brief Send text message
     * @param message 
     */
    void write(const std::string& message);

    /**
     * @brief Send text request
     */
    void write(text_request_ptr);

    /**
     * @brief Send join room request
     */
    void write(join_room_request_ptr);

    /**
     * @brief Setter for client_id
     * @param id 
     */
    void set_login_id(identifier_t id)   { client_id = id;}

    /**
     * @brief Login getter
     * @return const char* 
     */
    const char* get_login() const { return login; }

    /**
     * @brief Finish the communication with server
     */
    void close_connection();

    /**
     * @brief destructor
     */
    ~Client() {
        std::cout << "Destr client" << std::endl;
        close_connection();
    }

private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;
    const boost::asio::ip::tcp::resolver::results_type& eps;

    std::deque<packet_ptr> packets_to_server;

    char login[Block::LoginName];
    char password[Block::Password];
    identifier_t client_id;
    identifier_t room_id = 0;

private:
    /**
     * @brief Log in scenario implementation
     * @return input_request_ptr 
     */
    [[deprecated]]
    input_request_ptr logon();

    /**
     * @brief Start connection to the server
     */
    void do_connect(const boost::asio::ip::tcp::resolver::results_type&, input_request_ptr);

    /**
     * @brief Log in on the server
     * @param request
     */
    void send_login_request(input_request_ptr request);

    /**
     * @brief Entry point for handling server response
     */
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
