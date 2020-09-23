#include "control/control.h"

Control::Control(int argc, char** argv) :
    start_window(std::make_unique<MainWindow>()),
    channels_window(std::make_unique<ChannelsWindow>()),
    chat_window(std::make_unique<ChatWindow>())
{
    qDebug() << "Ctor Control";
    if (argc>1) {
        ip = std::string(argv[1]);
        if (argc>2) {
            port = std::stoi(argv[2]);
        }
    }

    start_window->show();

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<InputCode>("InputCode");
    qRegisterMetaType<DateTime>("DateTime");
    qRegisterMetaType<ClientTextMsg>("ClientTextMsg");

    QObject::connect(start_window.get(), &MainWindow::send_autorisation_info, this, &Control::autorisation);
    QObject::connect(start_window.get(), &MainWindow::send_registration_info, this, &Control::registration);

    QObject::connect(channels_window.get(), &ChannelsWindow::sig_add_channel, this, &Control::change_room);
    QObject::connect(channels_window.get(), &ChannelsWindow::sig_join_room, this, &Control::go_channel);

//    QObject::connect(chat_window.get(), &ChatWindow::send_change_room, this, &Control::change_room);
    QObject::connect(this, &Control::send_text_to_gui, chat_window.get(), &ChatWindow::print_text);
    QObject::connect(chat_window.get(), &ChatWindow::send_text_data, this, &Control::get_text_from_gui);

    QObject::connect(chat_window.get(), &ChatWindow::go_channels_window, this, &Control::change_room_from_chat);
}

void Control::connect_to_server(const std::string& login, const std::string& password, TypeCommand command) {
    qDebug() <<  "connect_to_server()";

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve(ip, std::to_string(port));

    Protocol::ptr_proto_request_t ptr_request;
    if (command == TypeCommand::RegistrationRequest) {
        ptr_request = Protocol::MsgFactory::create_reg_request(login, password);
    }
    else if (command == TypeCommand::AuthorisationRequest) {
        ptr_request = Protocol::MsgFactory::create_input_request(login, password);
    }

    auto ptr_header = Protocol::MsgFactory::create_header(command, ptr_request->ByteSizeLong());
    auto bin_buffer = Protocol::MsgFactory::serialize_request(std::move(ptr_header), std::move(ptr_request));

    client = std::make_shared<Client>(io_service, endpoints);
    client->do_connect(std::move(bin_buffer));
    client->set_login(login);
    channels_window->set_client(client);
    chat_window->set_client(client);

    QObject::connect(client.get(), &Client::send_text, this, &Control::text_from_client);
    QObject::connect(client.get(), SIGNAL(send_input_code(InputCode)), this, SLOT(change_window(InputCode)));
    QObject::connect(client.get(), &Client::sig_update_channels, this, &Control::update_channels);

    std::thread th([&io_service]() {
        io_service.run();
    });
    th.join();

    if (client) {
        client->close_connection();
    }

    qDebug() << "close connect_to_server";
}

void Control::change_window(InputCode a_code) {
//    QObject::connect(this, SIGNAL(send_input_code(InputCode)), start_window.get(), SLOT(handler_input_code(InputCode)));
    if (a_code == InputCode::BusyRegistr || a_code == InputCode::IncorrectAutor) {
//        emit send_input_code(a_code);
        start_window->handler_input_code(a_code);
    }
    else {
        start_window->hide();
        channels_window->show();
    }
}

void Control::change_room_from_chat() {
    chat_window->hide();
    channels_window->show();
}
