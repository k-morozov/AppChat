#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include "client/client.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

signals:
    /**
     * @brief Notify user is sending message
     *
     * @param login user's login
     * @param text message's content
     * @param room_id room where user is subscriber
     */
    void send_text_data(const std::string& login, const std::string& text, int room_id);

    /**
     * @brief Change the room
     *
     * @param new_room_id id of the room user wants to join
     */
    void send_change_room(int new_room_id);

private slots:
    void on_push_send_clicked();

private slots:
    void on_push_change_room_id_clicked();

public slots:
    /**
     * @brief show incoming message
     *
     * @param login sender's login
     * @param message message's content
     * @param dt date and time of sending the text
     */
    void print_text(const std::string& login, const std::string& message, DateTime dt);

private:
    Ui::ChatWindow *ui;

    QString logon;
    QString password;
    QString roomid;
};

#endif // CHATWINDOW_H
