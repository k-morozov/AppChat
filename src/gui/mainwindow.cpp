#include <gui/mainwindow.h>
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->password->setEchoMode(QLineEdit::Password);
    ui->text_input->setReadOnly(true);
    ui->text_output->setReadOnly(true);
    ui->text_input->hide();
    ui->text_output->hide();
    ui->room_id->hide();
    ui->room_id->setReadOnly(true);
    ui->label_channel_id->hide();
    ui->push_send->hide();
    ui->push_change_room_id->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_push_autorisation_clicked()
{
    logon = ui->logon->text();
    password = ui->password->text();
    if (logon.isEmpty() || password.isEmpty()) return;
    send_autorisation_info(logon.toStdString(), password.toStdString());
}

void MainWindow::on_push_send_clicked()
{
    auto message = ui->text_input->text();
    send_text_data(logon.toStdString(), message.toStdString(), roomid.toUInt());

    ui->text_input->clear();
    ui->text_output->setTextColor(QColor(50,205,50));
    ui->text_output->append(logon + ": " + message);
    ui->text_output->setTextColor(QColor(0,0,0));
}


void MainWindow::print_text(const std::string& from, const std::string& text) {
    std::string s(from + ": " + text);
    QString message(s.data());
    if (from=="server") {
        ui->text_output->setTextColor(QColor(255,0,0));
        ui->text_output->append(message);
        ui->text_output->setTextColor(QColor(0,0,0));

    }
    else {
        ui->text_output->append(message);
    }
}

void MainWindow::on_push_registration_clicked()
{
    logon = ui->logon->text();
    password = ui->password->text();
    if (logon.isEmpty() || password.isEmpty()) return;
    send_registration_info(logon.toStdString(), password.toStdString());
}

void MainWindow::on_push_change_room_id_clicked()
{
    auto new_roomid = ui->room_id->text();
    if (new_roomid.isEmpty()) return;
    roomid = new_roomid;
    ui->text_output->clear();
    emit send_change_room(roomid.toInt());
}

void MainWindow::good_input() {
    ui->text_input->show();
    ui->text_output->show();
    ui->room_id->show();
    ui->label_channel_id->show();
    ui->push_send->show();
    ui->push_change_room_id->show();
    ui->room_id->setReadOnly(false);

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
}

void MainWindow::handler_input_code(InputCode code) {
    ui->logon->clear();
    ui->password->clear();

    switch (code) {
        case InputCode::RegistrOK:
            QMessageBox::information(this, "registration", "You successfully registered.");
            good_input();
        break;

        case InputCode::BusyRegistr:
            QMessageBox::information(this, "error from server", "this login is already registered.");
        break;

        case InputCode::AutorOK:
            good_input();
        break;

        case InputCode::IncorrectAutor:
            QMessageBox::information(this, "error from server", "check login/password");
        break;
    default:
        break;
    }
}
