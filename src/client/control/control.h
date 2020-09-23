#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include "client/client.h"
#include "gui/mainwindow.h"
#include "gui/chatwindow.h"
#include "gui/channelswindow.h"

#include "protocol/msgfactory.h"

#include <vector>
/**
 * @brief Controller
 * @param ip and port server
 */
class Control: public QObject
{
    Q_OBJECT
public:
    Control(int argc, char** argv);

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
         qDebug() << "Destr Control" ;
        if (client) {
            client->close_connection();
        }
    }

signals:
    /**
     * @brief Show reveived message
     */
    void send_text_to_gui(const ClientTextMsg& msg);

    void send_input_code(InputCode);

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
                 qDebug() << "exception from thread: " << ex.what();
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
                 qDebug() << "exception from thread: " << ex.what();
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
    void get_text_from_gui(const ClientTextMsg& msg) {
        qDebug() << "send msg to server";
        client->send_msg_to_server(msg.text, msg.channel_id);
    }

    /**
     * @brief Notify UI about received message
     * @param from sender's login
     * @param text message content
     * @param dt date and time of sending the text
     */
    void text_from_client(const ClientTextMsg& msg) {
        send_text_to_gui(msg);
    }

    /**
     * @brief Change chat room to another one
     * @param new_room_id room where user is switching
     */
    void change_room(int new_room_id) {
        client->change_room(new_room_id);
    }

    /**
     * @brief change_window
     * @todo IWindow - classes
     */
    void change_window(InputCode a_code);
    void change_room_from_chat();

    void update_channels() {
        if (channels_window) {
            channels_window->update_list_channels();
        }
    }

    void go_channel(int room_id) {
        if (chat_window) {
            client->set_current_channel(room_id);
            chat_window->upload_history();

            channels_window->hide();
            chat_window->show();
        }
    }

private:
    std::shared_ptr<Client> client;
    std::unique_ptr<MainWindow> start_window;
    std::unique_ptr<ChannelsWindow> channels_window;
    std::unique_ptr<ChatWindow> chat_window;

    std::vector<uint8_t> __buffer;

    /**
     * @todo convert to 4 bytes
     */
    std::string ip = "127.0.0.1";
    int32_t port = SERVER_DEFAULT_PORT;
};

#endif // CONTROL_H
