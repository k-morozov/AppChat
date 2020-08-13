#include "client/control/control.h"

Control::Control(const boost::program_options::variables_map& vm) {
    // use cmd args if exist
    if (vm.count("ip")) {
      m_ip = vm["ip"].as<std::string>();
      std::cout << "parsed ip:" << vm["ip"].as<std::string>() << std::endl;
    }
    if(vm.count("port")) {
      m_port = vm["port"].as<int32_t>();
      std::cout << "parsed port:" << vm["port"].as<int32_t>() << std::endl;
    }

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<InputCode>("InputCode");
    qRegisterMetaType<DateTime>("DateTime");

    QObject::connect(&w, &MainWindow::send_autorisation_info, this, &Control::autorisation);
    QObject::connect(&w, &MainWindow::send_registration_info, this, &Control::registration);
    QObject::connect(&w, &MainWindow::send_change_room, this, &Control::change_room);
    QObject::connect(this, &Control::send_text_to_gui, &w, &MainWindow::print_text);
    QObject::connect(&w, &MainWindow::send_text_data, this, &Control::get_text_from_gui);

    w.show();
}

void Control::connect_to_server(const std::string& login, const std::string& password, TypeCommand command) {
    std::cout << "open connect_to_server" << std::endl;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve(m_ip, std::to_string(m_port));

    input_request_ptr request;
    if (command==TypeCommand::RegistrationRequest) {
        request = std::make_shared<RegistrationRequest>(login, password);
    } else {
        request = std::make_shared<AutorisationRequest>(login, password);
    }
    client = std::make_unique<Client>(io_service, endpoints, request);

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
