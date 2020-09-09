#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QWidget>
#include <QtWidgets>
#include <QtCore>
#include <QCloseEvent>

#include "client/client.h"

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
