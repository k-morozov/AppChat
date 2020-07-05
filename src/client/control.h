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
        qRegisterMetaType<QTextCursor>("QTextCursor");

        QObject::connect(this, SIGNAL(send_text_to_gui(const std::string&, const std::string&)),
                         &w, SLOT(print_text(const std::string&, const std::string&)));
        QObject::connect(&w, SIGNAL(send_text_data(const std::string&, const std::string&, int)),
                         this, SLOT(get_text_from_gui(const std::string&, const std::string&, int)));
    }

    void work_client(const std::string& login, const std::string& password) {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        auto endpoints = resolver.resolve("127.0.0.1", "7777");

        input_request_ptr request  = std::make_shared<AutorisationRequest>(login, password);
        client = std::make_unique<Client>(io_service, endpoints, request);
        QObject::connect(client.get(), SIGNAL(send_text(const std::string&, const std::string&)),
                         this, SLOT(text_from_client(const std::string&, const std::string&)));

        std::thread th([&io_service]() {
            io_service.run();
        });
        th.join();
        client->close();
    }

    ~Control() {}

signals:
    void send_text_to_gui(const std::string&, const std::string&);

public slots:
    void autorisation(const std::string& login, const std::string& password, int /*room_id*/) {
        std::thread th([this, login, password]() {
            work_client(login, password);
        });
        th.detach();

//        th.join();
//        client.close();
    }

    void get_text_from_gui(const std::string& login, const std::string& text, int room_id) {
        text_request_ptr request = std::make_shared<TextRequest>(login, room_id, text);
        client->write(request);
    }

    void text_from_client(const std::string& from, const std::string& text) {
        std::cout << "text_from_client" << from << " " << text << std::endl;
        send_text_to_gui(from, text);
    }


private:
    std::unique_ptr<Client> client;
    MainWindow &w;
};

#endif // CONTROL_H
