#include "client/client/client.h"

void Client::close_connection() {
    std::cout << "Close connection" << std::endl;
    io_service.reset();
    if(sock.is_open()) {
        sock.close();
    }
    packets_to_server.clear();
    std::memset(login, 0, Block::LoginName);
    std::memset(password, 0, Block::Password);
    client_id = -1;
    room_id = 0;
}

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
    std::cout << "start do_connect" << std::endl;
    boost::asio::async_connect(sock, eps,
        [this, request](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
           if (!ec) {
               send_login_request(request);
               std::cout << "finish do_connect" << std::endl;
           }
           else {
               std::cout << "error do_connect()" << std::endl;
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

void Client::send_login_request(input_request_ptr request) {
    boost::system::error_code error_code;
    boost::asio::write(sock, boost::asio::buffer(request->get_header(),
                                                 Block::Header), error_code);
    if (error_code) {
        std::cout << "error send_login_request(header)" << std::endl;
        close_connection();
        return ;
    }
    boost::asio::write(sock, boost::asio::buffer(request->get_data(),
                                                 request->get_length_data()), error_code);
    if (error_code) {
        std::cout << "error send_login_request(data)" << std::endl;
        close_connection();
        return ;
    }
    std::cout << "login=" << request->get_login() << ", password=" << request->get_password()
              << ", time=" << request->get_datetime().to_simple_date() << std::endl;
    if (error_code) {
        std::cout << "error when send input_request" << std::endl;
        close_connection();
        return ;
    }

    input_res_ptr response = std::make_shared<InputResponse>();
    boost::asio::read(sock, boost::asio::buffer(response->get_header(),
                                                Block::Header), error_code);
    if (error_code) {
        std::cout << "error when read response(input_res_ptr)" << std::endl;
        close_connection();
        return ;
    }
    if (response->get_type_data()==TypeCommand::RegistrationResponse
        || response->get_type_data()==TypeCommand::AutorisationResponse)
    {
        boost::asio::read(sock, boost::asio::buffer(response->get_data(),
                                                    response->get_length_data()), error_code);

        if (error_code) {
            std::cout << "error when read response(data)" << std::endl;
            close_connection();
            return ;
        }
        if (response->get_type_data()==TypeCommand::RegistrationResponse)
            if (response->get_loginid()==-1) {
                emit send_input_code(InputCode::BusyRegistr);
                std::cout << "another login" << std::endl;
                close_connection();
                return;
            } else {
                emit send_input_code(InputCode::RegistrOK);
            }
        else {
            if (response->get_loginid()==-1) {
                emit send_input_code(InputCode::IncorrectAutor);
                std::cout << "Not found login/password" << std::endl;
                close_connection();
                return;
            }
            else {
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
        std::cout << " No innput response from server" << std::endl;
        close_connection();
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
                std::cout << "Error read_response_header()" << std::endl;
                close_connection();
            }
    });
}

void Client::read_response_data(registr_response_ptr packet) {
    std::cout << get_command_str(packet->get_type_data()) << ": ";

    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
                read_response_header();
            }
            else {
                std::cout << "Error read_response_data(registr)" << std::endl;
                close_connection();
            }
    });
}

void Client::read_response_data(autor_response_ptr packet) {
    std::cout << get_command_str(packet->get_type_data()) << ": ";

    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
                read_response_header();
            }
            else {
                std::cout << "Error read_response_data(autor)" << std::endl;
                close_connection();
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
                std::cout << "Error read_response_data(text)" << std::endl;
                close_connection();
            }
    });
}

void Client::send_request_header() {
    boost::asio::async_write(sock, boost::asio::buffer(packets_to_server.front()->get_header(), Block::Header),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            send_request_data();
        }
        else {
            std::cout << "Error send_request_header()" << std::endl;
            close_connection();
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
            std::cout << "Error send_request_data()" << std::endl;
            close_connection();
        }
    });
}
