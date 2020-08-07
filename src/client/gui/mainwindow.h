#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QWidget>
#include <QtWidgets>
#include <QtCore>
#include <QCloseEvent>

#include "client/client/client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief MainWindow
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construct ManWindow
     * 
     * @param parent QObejct parent that is neccessary to support Qt memory system
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroy the Main Window object
     * 
     */
    ~MainWindow();

    /**
     * @brief Get message from UI
     * 
     * @return std::string 
     */
    std::string get_message();
signals:
    /**
     * @brief Notify user is authorizing on the server
     * 
     * @param login user's login
     * @param password user's password
     */
    void send_autorisation_info(const std::string& login, const std::string& password);

    /**
     * @brief Notify user is registering on the server
     * 
     * @param login user's login
     * @param password user's password
     */
    void send_registration_info(const std::string& login, const std::string& password);

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

public slots:
    /**
     * @brief show incoming message
     * 
     * @param login sender's login
     * @param message message's content
     * @param dt date and time of sending the text
     */
    void print_text(const std::string& login, const std::string& message, DateTime dt);

private slots:
    /**
     * @brief Handle user pressed "Authorize"
     */
    void on_push_autorisation_clicked();

    /**
     * @brief Handle user pressed "Register"
     */
    void on_push_registration_clicked();

    /**
     * @brief Handle user pressed "Clicked"
     */
    void on_push_send_clicked();
    
    /**
     * @brief Handle user clicked change room
     */
    void on_push_change_room_id_clicked();


    /**
     * @brief Handle correct input
     */
    void good_input();
    
    /**
     * @brief Handle received input
     * 
     * @param input_code
     */
    void handler_input_code(InputCode input_code);
private:
    Ui::MainWindow *ui;

    QString logon;
    QString password;
    QString roomid;
};
#endif // MAINWINDOW_H
