#include <string>
#include <deque>
#include <boost/asio.hpp>
#include <log/logger.h>
#include <protocol/protocol.h>

class Client {
public:
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps)
        : io_service(io), sock(io), flag_logon(false)
    {
        logon();
        do_connect(eps);
    }


    void write(const Message& mes) {
        bool process_write = !sending_message.empty();
        sending_message.push_back(mes);

        while (!flag_logon) {}

        if (!process_write) {
            do_write();
        }
    }

    void close() {
        boost::asio::post(io_service, [this]() {
            sock.close();
        });
    }

private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;

    Message receiving_message;
    std::deque<Message> sending_message;

    char login[32];
    volatile bool flag_logon;
private:
    void logon() {
        std::cout << "Enter your login: ";
        std::cin.getline(login, 32);
    }

    void do_connect(const boost::asio::ip::tcp::resolver::results_type& eps) {
        boost::asio::async_connect(sock, eps,
            [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
               if (!ec) {
                   send_login(Message(login));
               }
        });
    }

    void send_login(const Message& mes) {
//        std::cout << " send_login " << mes.get_body() << std::endl;
        do_send_login(mes);
    }


    void do_send_login(const Message& message) {
        boost::asio::async_write(sock, boost::asio::buffer(message.get_data(), message.get_mes_length()),
            [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
//                std::cout << "send login" << std::endl;
                flag_logon = true;
                do_read_header();
            }
            else {
                sock.close();
            }
        });
    }
    void do_read_header() {
        boost::asio::async_read(sock, boost::asio::buffer(receiving_message.get_data(), Message::header_size),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    if (receiving_message.decode_header())
                    {
                        auto size_body = receiving_message.get_body_length();
                        do_read_body(size_body);
                    }
                }
                else {
                    sock.close();
                }
        });
    }
    void do_read_body(std::size_t size_body) {
        boost::asio::async_read(sock, boost::asio::buffer(receiving_message.get_body(), size_body),
            [this](boost::system::error_code error, std::size_t) {
                if (!error) {
                    *(receiving_message.get_body()+receiving_message.get_body_length()) = '\0';
                    std::cout << receiving_message.get_body() << std::endl;
                    do_read_header();
                }
                else {
                    sock.close();
                }
        });
    }
    void do_write() {
        boost::asio::async_write(sock, boost::asio::buffer(sending_message.front().get_data(), sending_message.front().get_mes_length()),
            [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
//                std::cout << "send: " << receiving_message.get_body() << std::endl;
                sending_message.pop_front();
                if (!sending_message.empty()) do_write();
            }
            else {
                sock.close();
            }
        });
    }
};

int main() {
    std::cout << "start client..." << std::endl;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve("127.0.0.1", "7777");

    Client client(io_service, endpoints);
    std::thread th([&io_service]() { io_service.run();} );

    char mes[Message::max_body_size+1];
    while(std::cin.getline(mes, Message::max_body_size+1) ) {
        client.write(Message(mes));
    }

    th.join();
    client.close();
}
