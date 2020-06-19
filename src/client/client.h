#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <deque>
#include <boost/asio.hpp>
#include <protocol/protocol.h>

class Client {
public:
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps)
        : io_service(io), sock(io)
    {
        logon();
        do_connect(eps);
    }

    void write(Message mes);

    void close() {
        boost::asio::post(io_service, [this]() {
            sock.close();
        });
    }
    void set_id(int id) {client_id = id;}
    const char* get_login() const { return login; }
private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;

    Message receiving_message;
    std::deque<Message> sending_message;

    char login[Message::login_str_size];
    int32_t client_id;
private:
    void logon() {
        std::cout << "Enter your login: ";
        std::cin.getline(login, 32);
    }

    void do_connect(const boost::asio::ip::tcp::resolver::results_type& eps);

    void send_login(const Message& mes) {
//        std::cout << " send_login " << mes.get_body() << std::endl;
        do_send_login(mes);
    }


    void do_send_login(const Message& message);
    void do_read_header();
    void do_read_body(std::size_t size_body);
    void do_write();
};

#endif // CLIENT_H
