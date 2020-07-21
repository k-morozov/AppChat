#include <client/client.h>
//#include <charconv>

void Client::write(const std::string& message) {
    text_request_ptr text_request = std::make_shared<TextRequest>(login, room_id, message);

    bool process_write = !packets_to_server.empty();
    packets_to_server.push_back(text_request);

    if (!process_write) {
        send_request_header();
    }
}

void Client::write(text_request_ptr request) {
    bool process_write = !packets_to_server.empty();
    packets_to_server.push_back(request);

    if (!process_write) {
        send_request_header();
    }
}
void Client::write(join_room_request_ptr request) {
    bool process_write = !packets_to_server.empty();
    packets_to_server.push_back(request);
    if (!process_write) {
        send_request_header();
    }
}

void Client::do_connect(const boost::asio::ip::tcp::resolver::results_type& eps, input_request_ptr request) {
    boost::asio::async_connect(sock, eps,
        [this, request](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
           if (!ec) {
               send_login_packet(request);
           }
    });
}

input_request_ptr Client::logon() {
    std::cout << "Enter your login: ";
    std::cin.getline(login, Block::LoginName);
    std::cout << "Enter your password: ";
    std::cin.getline(password, Block::Password);
    std::cout << "enter room_id=";
    std::string room;
    std::cin.getline(room.data(), Block::Password);
    room_id = std::stoi(room);
    std::cout << "************************************" << std::endl;
//    }
    return std::make_shared<AutorisationRequest>(login, password);
}

void Client::send_login_packet(packet_ptr packet) {
    boost::system::error_code error_code;
    boost::asio::write(sock, boost::asio::buffer(packet->get_header(),
                                                 Block::Header), error_code);

    boost::asio::write(sock, boost::asio::buffer(packet->get_data(),
                                                 packet->get_length_data()), error_code);

    if (error_code) {
        sock.close();
        std::cout << "error when send login" << std::endl;
        return ;
    }

    input_res_ptr response = std::make_shared<InputResponse>();
    boost::asio::read(sock, boost::asio::buffer(response->get_header(),
                                                Block::Header), error_code);
    if (error_code) {
        sock.close();
        std::cout << "error when read login-id" << std::endl;
        return ;
    }
    if (response->get_type_data()==TypeCommand::RegistrationResponse
        || response->get_type_data()==TypeCommand::AutorisationResponse)
    {
        boost::asio::read(sock, boost::asio::buffer(response->get_data(),
                                                    response->get_length_data()), error_code);


        if (response->get_type_data()==TypeCommand::RegistrationResponse)
            if (response->get_loginid()==-1) {
//                emit bad_client_is_registred();
                emit send_input_code(InputCode::BusyRegistr);
                this->close();
                return;
            }
            else
            {
//                good_client_is_registred();
                emit send_input_code(InputCode::RegistrOK);
            }
        else {
            if (response->get_loginid()==-1) {
//                emit bad_client_is_autorisation();
                emit send_input_code(InputCode::IncorrectAutor);
                this->close();
                return;
            }
            else
            {
//                good_client_is_autorisation();
                emit send_input_code(InputCode::AutorOK);
            }
        }

        set_login_id(response->get_loginid());
        join_room_request_ptr request = std::make_shared<JoinRoomRequest>(room_id);
        write(request);
        if (!error_code) {
            read_response_header();
        }

    }
    else {
        std::cout << " No response from server" << std::endl;
    }

}

void Client::read_response_header() {
    response_ptr packet = std::make_shared<Response>();
    boost::asio::async_read(sock, boost::asio::buffer(packet->get_header(), Block::Header),
        [this, packet](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                switch (packet->get_type_data()) {
                    case TypeCommand::RegistrationRequest:
                        std::cout << "RegistrationRequest not been here" << std::endl;
                    break;
                    case TypeCommand::RegistrationResponse:
                        std::cout << "RegistrationResponse" << std::endl;
//                        read_response_data(std::make_shared<RegistrationResponse>(packet));
                        break;
                    case TypeCommand::AuthorisationRequest:
                        std::cout << "AuthorisationRequest not been here" << std::endl;
                        break;
                    case TypeCommand::AutorisationResponse:
                        read_response_data(std::make_shared<AutorisationResponse>(packet));
                        break;

                    case TypeCommand::EchoRequest:
                        std::cout << "EchoRequest: " << std::endl;
                    break;
                    case TypeCommand::EchoResponse:
                        read_response_data(std::make_shared<TextResponse>(packet));
                        break;

                    case TypeCommand::JoinRoomResponse:
                    default:
                        std::cout << "Unknown command " << packet->get_protocol_version() << std::endl;
                        break;
                }
            }
            else {
                sock.close();
            }
    });
}

void Client::read_response_data(registr_response_ptr packet) {
    std::cout << get_command_str(packet->get_type_data()) << ": ";

    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
//                std::cout << "read_response_data" << std::endl;

                read_response_header();
            }
            else {
                sock.close();
            }
    });
}

void Client::read_response_data(autor_response_ptr packet) {
    std::cout << get_command_str(packet->get_type_data()) << ": ";

    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
//                std::cout << "read_response_data" << std::endl;
                read_response_header();
            }
            else {
                sock.close();
            }
    });
}

void Client::read_response_data(text_response_ptr packet) {
    std::cout << get_command_str(packet->get_type_data()) << ": ";

    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
                auto local_time = DateTime::from_universal_to_local(packet->get_datetime());

                std::cout << packet->get_login() << ": " << packet->get_message() << std::endl;
                send_text(packet->get_login(), packet->get_message(), local_time);

                read_response_header();
            }
            else {
                sock.close();
            }
    });
}

//void Client::read_response_join_room(join_room_request_ptr) {

//}
void Client::send_request_header() {
    boost::asio::async_write(sock, boost::asio::buffer(packets_to_server.front()->get_header(), Block::Header),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            send_request_data();
        }
        else {
            std::cout << "error start_sending" << std::endl;
            sock.close();
        }
    });
}

void Client::send_request_data() {
    boost::asio::async_write(sock, boost::asio::buffer(packets_to_server.front()->get_data(),
                                                       packets_to_server.front()->get_length_data()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            packets_to_server.pop_front();
            if (!packets_to_server.empty()) send_request_header();
        }
        else {
            sock.close();
        }
    });
}
