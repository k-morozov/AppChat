#include <gui/mainwindow.h>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_push_autorisation_clicked()
{
    ui->text_output->append(QString("clicked"));
    auto logon = ui->logon->text();
    auto psw = ui->password->text();
    auto room = ui->room_id->text();
    ui->text_output->append(QString("send signal"));

    std::thread th([this, logon, psw, room]() {
        this->send_input_data(logon.toStdString(), psw.toStdString(), room.toUInt());
    });
    th.detach();
}

void MainWindow::on_push_send_clicked()
{
    auto message = ui->text_input->text();
    ui->text_input->clear();
    ui->text_output->append(message);
}


void MainWindow::print_text(const std::string& from, const std::string& text) {
    std::thread th([this, from, text]() {
        std::cout << "print_text" << std::endl;
        QString message = from.data();
        message.append(": ");
        message.append(text.data());
        ui->text_output->append(message);
    });
    th.detach();

}
