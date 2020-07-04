#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <client/client.h>
#include <gui/mainwindow.h>

class Control: public QObject
{
    Q_OBJECT
public:
    Control(MainWindow& w) : w(w) {
        qRegisterMetaType<std::string>("std::string");

        QObject::connect(this, SIGNAL(send_text_to_gui(const std::string&, const std::string&)),
                         &w, SLOT(print_text(const std::string&, const std::string&)));
    }

signals:
    void send_text_to_gui(const std::string&, const std::string&);

public slots:
    void autorisation(const std::string& login, const std::string& password, int /*room_id*/) {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        auto endpoints = resolver.resolve("127.0.0.1", "7777");

        input_request_ptr request  = std::make_shared<AutorisationRequest>(login, password);
        Client client(io_service, endpoints, request);
        QObject::connect(&client, SIGNAL(send_text(const std::string&, const std::string&)),
                         this, SLOT(text_from_client(const std::string&, const std::string&)));
        io_service.run();
        // not work
//        std::thread th([&io_service]() { io_service.run(); });
//        th.join();
        client.close();
    }

    void text_from_client(const std::string& from, const std::string& text) {
        std::cout << "text_from_client" << std::endl;

        std::thread th([this, from, text]() {
            this->send_text_to_gui(from, text);
        });
        th.detach();
    }


private:
//    std::unique_ptr<Client> client;
    MainWindow &w;
};

#endif // CONTROL_H
