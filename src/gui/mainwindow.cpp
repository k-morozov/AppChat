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
    ui->logon->setReadOnly(true);
    ui->password->setReadOnly(true);
    ui->room_id->setReadOnly(true);
    ui->push_autorisation->setHidden(true);

    ui->text_input->setReadOnly(false);
    ui->text_output->setReadOnly(false);

    logon = ui->logon->text();
    password = ui->password->text();
    roomid = ui->room_id->text();
    ui->text_output->append("send: login=" + logon + ", password=" + roomid + ", room_id=" +roomid);

    send_input_data(logon.toStdString(), password.toStdString(), roomid.toUInt());
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
//    std::cout << "MainWindow::ptint_text: " << s << std::endl;
    QString message(s.data());
    ui->text_output->append(message);
}
