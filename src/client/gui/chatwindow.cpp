#include "chatwindow.h"
#include "ui_chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::on_push_change_room_id_clicked()
{
    auto new_roomid = ui->room_id->text();
    if (new_roomid.isEmpty()) return;
    // @todo change logic: room change after ack from join_room_response()
    roomid = new_roomid;
    ui->text_output->clear();
    emit send_change_room(roomid.toInt());
}

void ChatWindow::on_push_send_clicked()
{
    using namespace boost::posix_time;

    auto message = ui->text_input->text();
    send_text_data(logon.toStdString(), message.toStdString(), roomid.toUInt());

    ui->text_input->clear();
    ui->text_output->setTextColor(QColor(50,205,50));
    ui->text_output->append("[" + QString::fromUtf8(to_simple_string(second_clock::local_time().time_of_day()).c_str()) + "] " + logon + ": " + message);
    ui->text_output->setTextColor(QColor(0,0,0));
}

void ChatWindow::print_text(const std::string& from, const std::string& text, DateTime dt) {
    std::string s("[" + dt.to_simple_time() + "] " + from + ": " + text);
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
