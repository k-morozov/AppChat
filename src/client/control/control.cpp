#include "control/control.h"

Control::Control(int argc, char** argv) {
    std::cout << "Ctor Control" << std::endl;
    if (argc>1) {
        ip = std::string(argv[1]);
        if (argc>2) {
            port = std::stoi(argv[2]);
        }
    }

    w.show();
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<InputCode>("InputCode");
    qRegisterMetaType<DateTime>("DateTime");

    QObject::connect(&w, &MainWindow::send_autorisation_info, this, &Control::autorisation);
    QObject::connect(&w, &MainWindow::send_registration_info, this, &Control::registration);
    QObject::connect(&w, &MainWindow::send_change_room, this, &Control::change_room);
    QObject::connect(this, &Control::send_text_to_gui, &w, &MainWindow::print_text);
    QObject::connect(&w, &MainWindow::send_text_data, this, &Control::get_text_from_gui);
}

void Control::connect_to_server(const std::string& login, const std::string& password, TypeCommand command) {
    std::cout << "connect_to_server()" << std::endl;
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

    QObject::connect(client.get(), &Client::send_text, this, &Control::text_from_client);
    QObject::connect(client.get(), SIGNAL(send_input_code(InputCode)), &w, SLOT(handler_input_code(InputCode)));

    std::thread th([&io_service]() {
        io_service.run();
    });
    th.join();

    if (client) {
        client->close_connection();
    }
    std::cout << "close connect_to_server" << std::endl;
}
