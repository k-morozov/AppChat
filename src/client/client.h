#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <deque>
#include <boost/asio.hpp>
#include <protocol/protocol.h>

class Client {
public:
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps)
        : io_service(io), sock(io)
    {
        do_connect(eps);
    }

    void write(Message mes);

    void close() {
        boost::asio::post(io_service, [this]() {
            sock.close();
        });
    }
    void set_login_id(int id)   { client_id = id;}
    const char* get_login() const { return login; }
private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;

    Message receiving_message;
    std::deque<Message> sending_message;

    char login[Block::LoginName];
    char password[Block::Password];
    int32_t client_id;
    int32_t room_id;

private:
    input_req_ptr logon();
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& eps);

    void send_input_request(request_ptr request);
    void do_read_header();
    void do_read_body(std::size_t size_body);
    void do_write();
};

#endif // CLIENT_H
