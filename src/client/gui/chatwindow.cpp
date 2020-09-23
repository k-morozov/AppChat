#include "chatwindow.h"
#include "ui_chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

//    ui->label_channel_id->setStyleSheet("color: rgb(255, 250, 250)");

    QPalette pail;
    pail.setColor(this->backgroundRole(), Qt::darkCyan);
//    pail.setBrush(this->backgroundRole(), QBrush(QPixmap(":/theme/gui/images/chatwindow.jpg")));
    this->setPalette(pail);
//    this->setAutoFillBackground(true);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}


void ChatWindow::on_push_send_clicked()
{
    using namespace boost::posix_time;

    auto message = ui->text_input->text();
    ClientTextMsg msg {
        client_ptr->get_login(),
        message.toStdString(),
        client_ptr->get_current_channel(),
        DateTime()
    };
    send_text_data(msg);

    ui->text_input->clear();
//    ui->text_output->setTextColor(QColor(50,205,50));
//    ui->text_output->append(/*"[" + QString::fromUtf8(to_simple_string(second_clock::local_time().time_of_day()).c_str()) + "] " + */
//                            /*msg.author + ": " + */message);
//    ui->text_output->setTextColor(QColor(0,0,0));
}

void ChatWindow::print_text(const ClientTextMsg& msg) {
    if (msg.channel_id == client_ptr->get_current_channel()) {
        std::string s(/*"[" + msg.dt.to_simple_time() + "] " +*/ msg.author + ": " + msg.text);
        QString message(s.data());
        if (msg.author=="server") {
            ui->text_output->setTextColor(QColor(255,0,0));
            ui->text_output->append(message);
            ui->text_output->setTextColor(QColor(0,0,0));
        }
        else {
            ui->text_output->append(message);
        }
    }

}

void ChatWindow::upload_history() {
    ui->text_output->clear();
    for(const auto& msg : client_ptr->get_history(client_ptr->get_current_channel())) {
        print_text(msg);
    }
}

void ChatWindow::on_button_back_clicked()
{
    emit go_channels_window();
}
