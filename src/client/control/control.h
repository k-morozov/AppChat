#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <QString>
#include <QDateTime>

#include "client/client.h"

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
    }

signals:
    /**
     * @brief Show reveived message
     * 
     * @param login message sender's login
     * @param text message content
     * @param dt date and time of sending the text
     */
    void send_text_to_gui(const QString& login, const QString& text, const QDateTime dt);

    void registrationOk();
    void registrationBusy();
    void authorisationOk();
    void authorisationIncorrect();

public slots:
    /**
     * @brief User autorization
     * 
     * @param login user's login
     * 
     * @param password user's password
     */
    void authorisation(const QString& login, const QString& password) {
        std::thread th([this, login, password]() {
            connect_to_server(login.toStdString(), password.toStdString(),
                              TypeCommand::AuthorisationRequest);
        });
        th.detach();
    }

    /**
     * @brief User registration
     * 
     * @param login user's login
     * @param password user's password
     */
    void registration(const QString& login, const QString& password) {
        std::thread th([this, login, password]() {
            connect_to_server(login.toStdString(), password.toStdString(),
                              TypeCommand::RegistrationRequest);
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
    void get_text_from_gui(const QString& login, const QString& text, int room_id) {
        client->write(std::make_shared<TextRequest>(login.toStdString(), room_id, text.toStdString()));
    }

    /**
     * @brief Notify UI about received message
     * 
     * @param from sender's login
     * @param text message content
     * @param dt date and time of sending the text
     */
    void text_from_client(const std::string& from, const std::string& text, DateTime dt) {
        const QDateTime qdt(QDate(dt.year, dt.month, dt.day),
                            QTime(dt.hours, dt.minutes, dt.seconds));
        send_text_to_gui(QString(from.c_str()), QString(text.c_str()), qdt);
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

    void send_input_code(InputCode code);

private:
    std::unique_ptr<Client> client;
};

#endif // CONTROL_H
