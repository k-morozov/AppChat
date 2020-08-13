#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include <QWidget>
#include "client/client/client.h"
#include "client/gui/mainwindow.h"
#include <boost/program_options/variables_map.hpp>

/**
 * @brief Controller
 * @param ip and port server
 */
class Control: public QObject
{
    Q_OBJECT
public:
    Control(const boost::program_options::variables_map& vm);

    /**
     * @brief Start communication with server
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
        std::cout << "Destr Control" << std::endl;
        if (client) {
            client->close_connection();
        }
    }

signals:
    /**
     * @brief Show reveived message
     * @param login message sender's login
     * @param text message content
     * @param dt date and time of sending the text
     */
    void send_text_to_gui(const std::string& login, const std::string& text, DateTime dt);

public slots:
    /**
     * @brief User autorization
     * @param login user's login
     * @param password user's password
     * @todo fix typo autorisation -> authorization
     */
    void autorisation(const std::string& login, const std::string& password) {
        std::thread th([this, login, password]() {
            try {
                connect_to_server(login, password, TypeCommand::AuthorisationRequest);
            } catch (std::exception &ex) {
                std::cout << "exception from thread: " << ex.what() << std::endl;;
            }
        });
        th.detach();
    }

    /**
     * @brief User registration
     * @param login user's login
     * @param password user's password
     * @todo replace to async?
     */
    void registration(const std::string& login, const std::string& password) {
        std::thread th([this, login, password]() {
            try {
                connect_to_server(login, password, TypeCommand::RegistrationRequest);
            } catch (std::exception &ex) {
                std::cout << "exception from thread: " << ex.what() << std::endl;;
            }
        });
        th.detach();
    }

    /**
     * @brief Send message
     * @param login sender's login
     * @param text message content
     * @param room_id sender's room
     */
    void get_text_from_gui(const std::string& login, const std::string& text, int room_id) {
        client->write(std::make_shared<TextRequest>(login, room_id, text));
    }

    /**
     * @brief Notify UI about received message
     * @param from sender's login
     * @param text message content
     * @param dt date and time of sending the text
     */
    void text_from_client(const std::string& from, const std::string& text, DateTime dt) {
        send_text_to_gui(from, text, dt);
    }

    /**
     * @brief Change chat room to another one
     * @param new_room_id room where user is switching
     */
    void change_room(int new_room_id) {
        client->write(std::make_shared<JoinRoomRequest>(new_room_id));
    }

private:
    std::unique_ptr<Client> client;
    MainWindow w;

    /**
     * @todo convert to 4 bytes
     */
    std::string m_ip = "127.0.0.1";
    int32_t m_port = SERVER_DEFAULT_PORT;
};

#endif // CONTROL_H
