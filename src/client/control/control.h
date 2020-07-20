#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <client/client.h>
#include <gui/mainwindow.h>

/**
 * @brief Controller
 */
class Control: public QObject
{
    Q_OBJECT
public:
    Control();

    /**
     * @brief Start communication with server
     * 
     * @param login user's login
     * @param password user's password
     * @param command request code that would have sent on opened connection
     * it could be AutorisationRequest or RegistrationRequest
     * @todo fix noreturn
     */
    void connect_to_server(const std::string& login, const std::string& password, TypeCommand command);

    /**
     * @brief Close client socket when destroy UI controller.
     */
    ~Control() {
        client->close();
    }

signals:
    /**
     * @brief Show reveived message
     * 
     * @param login message sender's login
     * 
     * @param text message content
     */
    void send_text_to_gui(const std::string& login, const std::string& text);

public slots:
    /**
     * @brief User autorization
     * 
     * @param login user's login
     * 
     * @param password user's password
     * 
     * @todo fix typo autorisation -> authorization
     */
    void autorisation(const std::string& login, const std::string& password) {
        std::thread th([this, login, password]() {
            connect_to_server(login, password, TypeCommand::AuthorisationRequest);
        });
        th.detach();
    }

    /**
     * @brief User registration
     * 
     * @param login user's login
     * @param password user's password
     */
    void registration(const std::string& login, const std::string& password) {
        std::thread th([this, login, password]() {
            connect_to_server(login, password, TypeCommand::RegistrationRequest);
        });
        th.detach();
    }

    /**
     * @brief Send message
     * 
     * @param login sender's login
     * @param text message content
     * @param room_id sender's room
     */
    void get_text_from_gui(const std::string& login, const std::string& text, int room_id) {
        client->write(std::make_shared<TextRequest>(login, room_id, text));
    }

    /**
     * @brief Notify UI about received message
     * 
     * @param from sender's login
     * @param text message content
     */
    void text_from_client(const std::string& from, const std::string& text) {
        send_text_to_gui(from, text);
    }

    /**
     * @brief Change chat room to another one
     * 
     * @param new_room_id room where user is switching
     */
    void change_room(int new_room_id) {
//        emit send_text_to_gui("server", "получил запрос на смену комнаты");
        client->write(std::make_shared<JoinRoomRequest>(new_room_id));
    }


private:
    std::unique_ptr<Client> client;
    MainWindow w;
};

#endif // CONTROL_H
