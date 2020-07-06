#include <client/control.h>

Control::Control(MainWindow& w) : w(w) {
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QTextCursor>("QTextCursor");

    QObject::connect(this, SIGNAL(send_text_to_gui(const std::string&, const std::string&)),
                     &w, SLOT(print_text(const std::string&, const std::string&)));
    QObject::connect(&w, SIGNAL(send_text_data(const std::string&, const std::string&, int)),
                     this, SLOT(get_text_from_gui(const std::string&, const std::string&, int)));
}

<<<<<<< HEAD
void Control::work_client(const std::string& login, const std::string& password, int room_id) {
=======
void Control::work_client(const std::string& login, const std::string& password) {
>>>>>>> dev
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve("127.0.0.1", "7777");

    input_request_ptr request  = std::make_shared<AutorisationRequest>(login, password);
<<<<<<< HEAD
    client = std::make_unique<Client>(io_service, endpoints, request, room_id);
=======
    client = std::make_unique<Client>(io_service, endpoints, request);
>>>>>>> dev
    QObject::connect(client.get(), SIGNAL(send_text(const std::string&, const std::string&)),
                     this, SLOT(text_from_client(const std::string&, const std::string&)));

    std::thread th([&io_service]() {
        io_service.run();
    });
    th.join();
    client->close();
}
