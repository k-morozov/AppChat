#include <gui/mainwindow.h>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->password->setEchoMode(QLineEdit::Password);
    ui->text_input->setReadOnly(true);
    ui->text_output->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_push_autorisation_clicked()
{
    logon = ui->logon->text();
    password = ui->password->text();
//    roomid = ui->room_id->text();

    if (logon.isEmpty() || password.isEmpty() /*|| roomid.isEmpty()*/) return;

    ui->logon->setReadOnly(true);
    ui->password->setReadOnly(true);
//    ui->room_id->setReadOnly(true);
    ui->label_login->setHidden(true);
    ui->label_password->setHidden(true);

    ui->logon->setHidden(true);
    ui->password->setHidden(true);
//    ui->room_id->setHidden(true);

    ui->push_autorisation->setHidden(true);
    ui->push_registration->setHidden(true);

    ui->text_input->setReadOnly(false);

    send_input_data(logon.toStdString(), password.toStdString(), 0);

    ui->text_output->append(logon + " is autorisation");
}

void MainWindow::on_push_send_clicked()
{
    auto message = ui->text_input->text();
    ui->text_input->clear();
    ui->text_output->append(message);

    send_text_data(logon.toStdString(), message.toStdString(), roomid.toUInt());
}


void MainWindow::print_text(const std::string& from, const std::string& text) {
    std::string s(from + ": " + text);
    QString message(s.data());
    ui->text_output->append(message);
}

void MainWindow::on_push_registration_clicked()
{
    logon = ui->logon->text();
    password = ui->password->text();
    roomid = ui->room_id->text();

    if (logon.isEmpty() || password.isEmpty() || roomid.isEmpty()) return;

    ui->logon->setReadOnly(true);
    ui->password->setReadOnly(true);
//    ui->room_id->setReadOnly(true);
    ui->label_login->setHidden(true);
    ui->label_password->setHidden(true);

    ui->logon->setHidden(true);
    ui->password->setHidden(true);
//    ui->room_id->setHidden(true);

    ui->push_autorisation->setHidden(true);
    ui->push_registration->setHidden(true);

    ui->text_input->setReadOnly(false);

    send_input_data(logon.toStdString(), password.toStdString(), 0);

    ui->text_output->append(logon + " is registration");
}

void MainWindow::on_push_change_room_id_clicked()
{
    auto new_roomid = ui->room_id->text();
    if (new_roomid.isEmpty()) return;
    roomid = new_roomid;
    ui->text_output->clear();
    emit send_change_room(roomid.toInt());
}
