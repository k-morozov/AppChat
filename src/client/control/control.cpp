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

    std::unique_ptr<Serialize::InRequest> in_request = std::make_unique<Serialize::InRequest>();
    in_request->set_login(login);
    in_request->set_password(password);

    Serialize::Request request;
    request.set_allocated_input_request(in_request.release());

    std::cout << "conencting: " << request.input_request().login() << " and " << request.input_request().password()
                  << ", size=" << sizeof(request) << std::endl;
    const std::size_t length = sizeof(request);

    Serialize::Header header;
    header.set_length(static_cast<google::protobuf::int32>(length));

    header.set_version(1);
    header.set_command(static_cast<google::protobuf::uint64>(TypeCommand::AuthorisationRequest));
    header.set_time(0);

    __buffer.resize(sizeof(header) + length);
    header.SerializeToArray(__buffer.data(), sizeof(header));
    request.SerializeToArray(__buffer.data()+sizeof(header), header.length());

    client = std::make_unique<Client>(io_service, endpoints, __buffer);

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
