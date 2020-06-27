#include <server/connection/connection.h>

void Connection::sendme(const std::string& from, const std::string& message) {
    bool write_in_progress = !packets_to_client.empty();
    text_response_ptr response = std::make_shared<TextResponse>(from, message);
    packets_to_client.push_back(response);
    if (!write_in_progress) {

        send_to_client();
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
                    std::cout << "RegistrationRequest" << std::endl;
                    read_input_request_body(std::make_shared<RegistrationRequest>(request));
                    break;
                case TypeCommand::RegistrationResponse:
                case TypeCommand::AuthorisationRequest:
                    std::cout << "AuthorisationRequest" << std::endl;
                    read_input_request_body(std::make_shared<AutorisationRequest>(request));
                    break;
                case TypeCommand::AutorisationResponse:
                case TypeCommand::EchoRequest:
                    std::cout << "EchoRequest" << std::endl;
                    read_text_request_body(std::make_shared<TextRequest>(request));
                    break;
                case TypeCommand::EchoResponse:
                case TypeCommand::JoinRoomRequest:
                case TypeCommand::JoinRoomResponse:
                case TypeCommand::LeaveRoomRequest:
                default:
                    break;
            }
        } else {
            std::cout << "error read_request_header" << std::endl;
            socket.close();
        }
    });


}

void Connection::read_input_request_body(input_req_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                login = request->get_login();
                password = request->get_password();
                std::cout << "login=" << request->get_login() << ", pwd=" << request->get_password() << std::endl;

                input_res_ptr response = std::make_shared<AutorisationResponse>(client_id);
                std::cout << "vers=" << response->get_protocol_version() << ", command="
                          << static_cast<uint16_t>(response->get_type_data())
                          << ", logid=" << response->get_loginid() << std::endl;
                boost::asio::write(socket, boost::asio::buffer(response->get_header(), Block::Header));
                boost::asio::write(socket, boost::asio::buffer(response->get_data(), response->get_length_data()));

                ChannelsManager::Instance().join(self, 0);
                read_request_header();
            }
            else {
                socket.close();
            }
    });
}

void Connection::read_text_request_body(text_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                login = request->get_login();
                auto roomid = request->get_roomid();
                auto text = request->get_message();
                std::cout << "message: login=" << login << ", roomid=" << roomid << ", text="<<text << std::endl;

                text_response_ptr response = std::make_shared<TextResponse>(login, text);
                ChannelsManager::Instance().send(response);

                read_request_header();
            }
            else {
                socket.close();
            }
    });

}



//void Connection::do_read_header() {
//    std::cout << "read header" << std::endl;
//    auto self(shared_from_this());
//    request_ptr request = std::make_shared<TextRequest>();
//    boost::asio::async_read(socket, boost::asio::buffer(request->get_header(), Block::Header),
//        [this, self, request](boost::system::error_code error, std::size_t) {
//            if (!error) {
//                if (true/*read_mes.decode_header()*/) {
//                    read_text_request_body(std::make_shared<TextRequest>(request));
//                }
//            }
//            else {
//                std::cout << "need leave" << std::endl;
//                ChannelsManager::Instance().leave(self);
//            }
//        });
//}

//void Connection::do_read_body(request_ptr request) {
//    auto self(shared_from_this());
//    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
//        [this, self, request](boost::system::error_code error, std::size_t) {
//            if (!error) {
//                std::cout << std::dynamic_pointer_cast<TextRequest>(request)->get_login() << ": " <<
//                          std::dynamic_pointer_cast<TextRequest>(request)->get_message() << std::endl;
//                    ChannelsManager::Instance().send(read_mes);
//                do_read_header();
//            }
//            else {
//                ChannelsManager::Instance().leave(self);
//            }
//        }
//    );
//}

void Connection::send_to_client() {
    auto self(shared_from_this());
    boost::asio::async_write(socket,
        boost::asio::buffer(packets_to_client.front()->get_header(),
                            Block::Header),
            [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    send_data();
                } else {
                    std::cout << "error send_to_client()" << std::endl;
                    ChannelsManager::Instance().leave(self);
                }
            }
    );
}

void Connection::send_data() {
    auto self(shared_from_this());
    boost::asio::async_write(socket,
        boost::asio::buffer(packets_to_client.front()->get_data(),
                            packets_to_client.front()->get_length_data()),
            [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    packets_to_client.pop_front();
                    if (!packets_to_client.empty()) {
                        send_to_client();
                    }
                } else {
                    std::cout << "error send_data()" << std::endl;
                    ChannelsManager::Instance().leave(self);
                }
            }
    );
}
