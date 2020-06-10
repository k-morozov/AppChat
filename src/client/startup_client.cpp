#include <iostream>
#include <string>
#include <deque>
#include <boost/asio.hpp>
#include "../protocol/protocol.h"

class Client {
public:
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps)
        : io_service(io), sock(io)
    {
        do_connect(eps);
    }

    void write(const Messages& mes) {
//        std::cout << "write header = " << mes.get_length() <<  std::endl;
        bool process_write = !write_mes.empty();
        write_mes.push_back(mes);
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
    Messages mes_read;
    std::deque<Messages> write_mes;

private:
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& eps) {
        boost::asio::async_connect(sock, eps,
            [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
               if (!ec) {
                   do_read_header();
               }
        });
    }

    void do_read_header() {
//        std::cout << "do_read_header" << std::endl;
        boost::asio::async_read(sock, boost::asio::buffer(mes_read.get_data(), Messages::header_size),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    auto size_body = mes_read.get_length();
                    do_read_body(size_body);
                }
                else {
                    sock.close();
                }
        });
    }
    void do_read_body(std::size_t size_body) {
        boost::asio::async_read(sock, boost::asio::buffer(mes_read.get_body(), size_body),
            [this](boost::system::error_code error, std::size_t) {
                if (!error) {
                    std::cout << mes_read.get_body() << std::endl;
                    do_read_header();
                }
                else {
                    sock.close();
                }
        });
    }
    void do_write() {
        std::cout << "---> " << write_mes.front().get_body() << " ---> " << write_mes.front().get_length() << std::endl;;

        boost::asio::async_write(sock, boost::asio::buffer(write_mes.front().get_data(), write_mes.front().get_length()),
            [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                write_mes.pop_front();
                if (!write_mes.empty()) do_write();
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

    char mes[Messages::max_body_size];
    std::string str;
    while(std::cin >> str) {
        client.write(Messages(str.data()));
    }

    th.join();
    client.close();
}
