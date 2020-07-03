#include <string>

#include <boost/asio.hpp>
#include <QApplication>

#include <protocol/protocol.h>
#include <client/client.h>
#include <gui/mainwindow.h>

void run_client() {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve("127.0.0.1", "7777");

    Client client(io_service, endpoints);
    std::thread th([&io_service]() { io_service.run();} );

    char message[Block::TextMessage+1];
    while(std::cin.getline(message, Block::TextMessage, '\n') ) {
        client.write(message);
    }

    th.join();
    client.close();
}

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
//    run_client();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    return 0;
}
