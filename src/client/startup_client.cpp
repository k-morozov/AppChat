#include <string>

#include <boost/asio.hpp>
#include <QApplication>

#include <protocol/protocol.h>
#include <client/client.h>

void run_client() {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve("127.0.0.1", "7777");

    Client client(io_service, endpoints);
    std::thread th([&io_service]() { io_service.run();} );

    char mes[Message::max_body_size+1];
//    std::cout << client.get_login() << ": ";
    while(std::cin.getline(mes, Message::max_body_size+1, '\n') ) {
        client.write(Message(mes));
//        std::cout << client.get_login() << ": ";
    }

    th.join();
    client.close();
}

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
    std::cout << "start client..." << std::endl;

    run_client();

    return 0;
}
