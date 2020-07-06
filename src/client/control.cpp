#include <client/control.h>

Control::Control() {
    w.show();

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QTextCursor>("QTextCursor");

    QObject::connect(&w, &MainWindow::send_autorisation_info, this, &Control::autorisation);
    QObject::connect(&w, &MainWindow::send_registration_info, this, &Control::registration);

    QObject::connect(&w, &MainWindow::send_change_room, this, &Control::change_room);

    QObject::connect(this, SIGNAL(send_text_to_gui(const std::string&, const std::string&)),
                     &w, SLOT(print_text(const std::string&, const std::string&)));

    QObject::connect(&w, &MainWindow::send_text_data, this, &Control::get_text_from_gui);


}

void Control::connect_to_server(const std::string& login, const std::string& password, TypeCommand command) {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve("127.0.0.1", "7777");

    input_request_ptr request;
    if (command==TypeCommand::RegistrationRequest) {
        request = std::make_shared<RegistrationRequest>(login, password);
    } else {
        request = std::make_shared<AutorisationRequest>(login, password);
    }
    client = std::make_unique<Client>(io_service, endpoints, request);

    QObject::connect(client.get(), &Client::send_text, this, &Control::text_from_client);
    QObject::connect(client.get(), SIGNAL(good_input()), &w, SLOT(good_input()));
    QObject::connect(client.get(), SIGNAL(bad_input()), &w, SLOT(bad_input()));

    std::thread th([&io_service]() {
        io_service.run();
    });
    th.join();
    client->close();
}
