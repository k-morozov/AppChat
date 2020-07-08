#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <client/client.h>
#include <gui/mainwindow.h>

class Control: public QObject
{
    Q_OBJECT
public:
    Control();

    // @todo noreturn???
    void connect_to_server(const std::string&, const std::string&, TypeCommand command);

    ~Control() {
        client->close();
    }

signals:
    void send_text_to_gui(const std::string&, const std::string&);

public slots:
    void autorisation(const std::string& login, const std::string& password) {
        std::thread th([this, login, password]() {
            connect_to_server(login, password, TypeCommand::AuthorisationRequest);
        });
        th.detach();
    }

    void registration(const std::string& login, const std::string& password) {
        std::thread th([this, login, password]() {
            connect_to_server(login, password, TypeCommand::RegistrationRequest);
        });
        th.detach();
    }

    void get_text_from_gui(const std::string& login, const std::string& text, int room_id) {
        client->write(std::make_shared<TextRequest>(login, room_id, text));
    }

    void text_from_client(const std::string& from, const std::string& text) {
        send_text_to_gui(from, text);
    }

    void change_room(int new_room_id) {
//        emit send_text_to_gui("server", "получил запрос на смену комнаты");
        client->write(std::make_shared<JoinRoomRequest>(new_room_id));
    }


private:
    std::unique_ptr<Client> client;
    MainWindow w;
};

#endif // CONTROL_H
