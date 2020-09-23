#include "gui/mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

#include <boost/date_time/posix_time/posix_time.hpp>

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label_username->setStyleSheet("color: rgb(255, 250, 250)");
    ui->label_password->setStyleSheet("color: rgb(255,250,250)");

    QPalette pail;
//    pail.setBrush(this->backgroundRole(), QBrush(QPixmap(":/theme/gui/images/mainwindow.jpg")));
    pail.setColor(this->backgroundRole(), Qt::darkCyan);
    this->setPalette(pail);
//    this->setAutoFillBackground(true);

    ui->password->setEchoMode(QLineEdit::Password);

    QWidget::setTabOrder(ui->logon, ui->password);
    QWidget::setTabOrder(ui->password, ui->push_autorisation);
    QWidget::setTabOrder(ui->push_autorisation, ui->push_registration);
    QWidget::setTabOrder(ui->push_registration, ui->logon);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_push_autorisation_clicked()
{
    const auto login = ui->logon->text().toStdString();
    const auto password = ui->password->text().toStdString();
    if (login.empty() || password.empty()) return;
    send_autorisation_info(login, password);
    ui->logon->clear();
    ui->password->clear();
}


void MainWindow::on_push_registration_clicked()
{
    const auto login = ui->logon->text().toStdString();
    const auto password = ui->password->text().toStdString();
    if (login.empty() || password.empty()) return;
    send_registration_info(login, password);
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
