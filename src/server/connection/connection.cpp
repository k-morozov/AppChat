#include <server/connection/connection.h>

void Connection::sendme(text_response_ptr response) {
    bool write_in_progress = !packets_to_client.empty();
    packets_to_client.push_back(response);
    if (!write_in_progress) {
        send_response_header();
    }
}

void Connection::read_request_header() {
    request_ptr request = std::make_shared<Request>();
    boost::asio::async_read(socket, boost::asio::buffer(request->get_header(), Block::Header),
                            [this, request](boost::system::error_code error, std::size_t) {
        if (!error) {
            switch (request->get_type_data()) {
                case TypeCommand::Unknown:
                    break;
                case TypeCommand::RegistrationRequest:
                    std::cout << get_command_str(request->get_type_data()) << "--> ";
                    //read_request_body(std::make_shared<RegistrationRequest>(request));
                    break;
                case TypeCommand::RegistrationResponse:
                case TypeCommand::AuthorisationRequest:
                    std::cout << get_command_str(request->get_type_data()) << "--> ";
                    read_request_body(std::make_shared<AutorisationRequest>(request));
                    break;
                case TypeCommand::AutorisationResponse:
                case TypeCommand::EchoRequest:
                    std::cout << get_command_str(request->get_type_data()) << "--> ";
                    read_request_body(std::make_shared<TextRequest>(request));
                    break;
                case TypeCommand::EchoResponse:
                case TypeCommand::JoinRoomRequest:
                    std::cout << get_command_str(request->get_type_data()) << "--> ";
                    read_request_body(std::make_shared<JoinRoomRequest>(request));
                    break;
                case TypeCommand::JoinRoomResponse:
                case TypeCommand::LeaveRoomRequest:
                default:
                    std::cout << get_command_str(request->get_type_data()) << "--> " << std::endl;;
                    break;
            }
        } else {
            ChannelsManager::Instance().leave(shared_from_this());
            socket.close();
        }
    });

}

void Connection::read_request_body(autor_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                login = request->get_login();
                password = request->get_password();
                std::cout << "login=" << request->get_login() << ", pwd=" << request->get_password() << std::endl;

                client_id = Database::Instance().get_loginid(login);
                if (client_id==-1) {
                    client_id = generate_client_id();
                }

                input_res_ptr response = std::make_shared<AutorisationResponse>(client_id);
                std::cout << "AutorisationResponse: vers=" << response->get_protocol_version() << ", command="
                          << get_command_str(response->get_type_data())
                          << ", logid=" << response->get_loginid() << std::endl;

                Database::Instance().add_logins(login, response->get_loginid(), password);
                boost::asio::write(socket, boost::asio::buffer(response->get_header(), Block::Header));
                boost::asio::write(socket, boost::asio::buffer(response->get_data(), response->get_length_data()));


                //ChannelsManager::Instance().join(self, 0);
                read_request_header();
            }
            else {
                ChannelsManager::Instance().leave(shared_from_this());
                socket.close();
            }
    });
}

void Connection::read_request_body(text_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                login = request->get_login();
                auto roomid = request->get_roomid();
                auto text = request->get_message();
                std::cout << "login=" << login << ", roomid=" << roomid << ", text="<<text << std::endl;
                text_response_ptr response = std::make_shared<TextResponse>(login, text, roomid);
                ChannelsManager::Instance().send(response);
                Database::Instance().save_text_message(request);

                read_request_header();
            }
            else {
                ChannelsManager::Instance().leave(shared_from_this());
                socket.close();
            }
    });

}

void Connection::read_request_body(join_room_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                auto roomid = request->get_roomid();
                std::cout << "roomid=" << roomid << std::endl;
                ChannelsManager::Instance().join(self, roomid);

                read_request_header();
            }
            else {
                ChannelsManager::Instance().leave(shared_from_this());
                socket.close();
            }
    });

}
void Connection::send_response_header() {
    auto self(shared_from_this());
    boost::asio::async_write(socket,
        boost::asio::buffer(packets_to_client.front()->get_header(),
                            Block::Header),
            [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    send_response_data();
                } else {
                    std::cout << "error send_response_header()" << std::endl;
                    ChannelsManager::Instance().leave(self);
                }
            }
    );
}

void Connection::send_response_data() {
    auto self(shared_from_this());
    boost::asio::async_write(socket,
        boost::asio::buffer(packets_to_client.front()->get_data(),
                            packets_to_client.front()->get_length_data()),
            [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    packets_to_client.pop_front();
                    if (!packets_to_client.empty()) {
                        send_response_header();
                    }
                } else {
                    std::cout << "error send_response_data()" << std::endl;
                    ChannelsManager::Instance().leave(self);
                }
            }
    );
}
