#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <deque>
#include <boost/asio.hpp>
#include <protocol/protocol.h>
#include <QWidget>

class Client: public QObject {
   Q_OBJECT

public:
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps,
<<<<<<< HEAD
           input_request_ptr request, int room_id)
        : io_service(io), sock(io), eps(eps), room_id(room_id)
=======
           input_request_ptr request)
        : io_service(io), sock(io), eps(eps)
>>>>>>> dev
    {
        do_connect(eps, request);
    }

    void write(const std::string& message);
    void write(text_request_ptr);
    void write(join_room_request_ptr);

    void close() {
        boost::asio::post(io_service, [this]() {
            sock.close();
        });
    }
    void set_login_id(identifier_t id)   { client_id = id;}
    const char* get_login() const { return login; }

<<<<<<< HEAD
=======
    ~Client() {

    }
>>>>>>> dev
private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;
    const boost::asio::ip::tcp::resolver::results_type& eps;

    std::deque<packet_ptr> packets_to_server;

    char login[Block::LoginName];
    char password[Block::Password];
    identifier_t client_id;
<<<<<<< HEAD
    identifier_t room_id = -1;
=======
    identifier_t room_id = 0;
>>>>>>> dev

private:
    input_request_ptr logon();
    void do_connect(const boost::asio::ip::tcp::resolver::results_type&, input_request_ptr);

    void send_login_packet(packet_ptr packet);

    void read_response_header();
    void read_response_data(autor_response_ptr);
    void read_response_data(text_response_ptr);

    void send_request_header();
    void send_request_data();


signals:
    void send_text(const std::string& from, const std::string& text);
};

#endif // CLIENT_H
