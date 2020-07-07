#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QWidget>
#include <QtWidgets>
#include <QtCore>

#include <client/client.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::string get_message();

signals:
    void send_autorisation_info(const std::string&, const std::string&);
    void send_registration_info(const std::string&, const std::string&);

    void send_text_data(const std::string&, const std::string&, int);
    void send_change_room(int);

private slots:
    void on_push_autorisation_clicked();
    void on_push_registration_clicked();

    void on_push_send_clicked();
    void print_text(const std::string&, const std::string&);
    void on_push_change_room_id_clicked();
    void good_input();

    // @todo add code error
    void good_client_is_registred();
    void bad_client_is_registred();
    void good_client_is_autorisation();
    void bad_client_is_autorisation();
private:
    Ui::MainWindow *ui;

    QString logon;
    QString password;
    QString roomid;
};
#endif // MAINWINDOW_H
