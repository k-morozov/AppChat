#include <iostream>
#include <string>
#include <deque>
#include <boost/asio.hpp>

class Client {
public:
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& eps)
        : io_service(io), sock(io)
    {
        do_connect(eps);
    }

    void write(std::string mes) {
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
    char mes_read[4];
    std::deque<std::string> write_mes;

private:
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& eps) {
        boost::asio::async_connect(sock, eps,
            [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
               if (!ec) {
                   do_read();
               }
        });
    }

    void do_read() {
        boost::asio::async_read(sock, boost::asio::buffer(mes_read, 4),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    std::cout << mes_read << std::endl;
                    do_read();
                }
                else {
                    sock.close();
                }
        });
    }

    void do_write() {
        boost::asio::async_write(sock, boost::asio::buffer(write_mes.front().data(), write_mes.front().size()),
            [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                write_mes.pop_front();
                do_write();
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

    std::string mes;
    while(getline(std::cin, mes)) {
        client.write(mes);
    }

    th.join();
    client.close();
}
