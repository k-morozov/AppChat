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

<<<<<<< HEAD
    send_input_data(logon.toStdString(), password.toStdString(), 0);

    ui->text_output->append(logon + " is autorisation");
=======
    send_input_data(logon.toStdString(), password.toStdString());

    ui->text_output->append("you is autorisation");
>>>>>>> dev
}

void MainWindow::on_push_send_clicked()
{
    auto message = ui->text_input->text();
<<<<<<< HEAD
    ui->text_input->clear();
    ui->text_output->append(message);

    send_text_data(logon.toStdString(), message.toStdString(), roomid.toUInt());
=======
    send_text_data(logon.toStdString(), message.toStdString(), roomid.toUInt());

    ui->text_input->clear();
    ui->text_output->setTextColor(QColor(50,205,50));
    ui->text_output->append(logon + ": " + message);
    ui->text_output->setTextColor(QColor(0,0,0));
>>>>>>> dev
}


void MainWindow::print_text(const std::string& from, const std::string& text) {
    std::string s(from + ": " + text);
    QString message(s.data());
<<<<<<< HEAD
    ui->text_output->append(message);
=======
    if (from=="server") {
        ui->text_output->setTextColor(QColor(255,0,0));
        ui->text_output->append(message);
        ui->text_output->setTextColor(QColor(0,0,0));

    }
    else {
        ui->text_output->append(message);
    }
>>>>>>> dev
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

<<<<<<< HEAD
    send_input_data(logon.toStdString(), password.toStdString(), 0);

    ui->text_output->append(logon + " is registration");
=======
    send_input_data(logon.toStdString(), password.toStdString());

    ui->text_output->append("you is registration");
>>>>>>> dev
}

void MainWindow::on_push_change_room_id_clicked()
{
    auto new_roomid = ui->room_id->text();
    if (new_roomid.isEmpty()) return;
    roomid = new_roomid;
    ui->text_output->clear();
    emit send_change_room(roomid.toInt());
}
