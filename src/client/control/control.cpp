#include "control.h"

Control::Control() {
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<InputCode>("InputCode");
    qRegisterMetaType<DateTime>("DateTime");
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
    QObject::connect(client.get(), &Client::send_input_code, this, &Control::send_input_code);

    std::thread th([&io_service] {
      io_service.run();
    });

    th.join();
    client->close();
}

void Control::send_input_code(InputCode code)
{
  switch (code) {
    case InputCode::RegistrOK: emit registrationOk(); break;
    case InputCode::BusyRegistr: emit registrationBusy(); break;
    case InputCode::AutorOK: emit authorisationOk(); break;
    case InputCode::IncorrectAutor: emit authorisationIncorrect(); break;
  }
}
