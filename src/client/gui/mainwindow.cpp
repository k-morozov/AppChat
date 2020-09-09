#include "gui/mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

#include <boost/date_time/posix_time/posix_time.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->password->setEchoMode(QLineEdit::Password);
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
    ui->logon->clear();
    ui->password->clear();
}


void MainWindow::on_push_registration_clicked()
{
    logon = ui->logon->text();
    password = ui->password->text();
    if (logon.isEmpty() || password.isEmpty()) return;
    send_registration_info(logon.toStdString(), password.toStdString());
    ui->logon->clear();
    ui->password->clear();
}


void MainWindow::handler_input_code(InputCode code) {
    ui->logon->clear();
    ui->password->clear();

    switch (code) {
        case InputCode::RegistrOK:
            QMessageBox::information(this, "registration", "You successfully registered.");
        break;

        case InputCode::BusyRegistr:
            QMessageBox::information(this, "error from server", "this login is already registered.");
        break;

        case InputCode::AutorOK:
        break;

        case InputCode::IncorrectAutor:
            QMessageBox::information(this, "error from server", "check login/password");
        break;
    default:
        break;
    }
}
