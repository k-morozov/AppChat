#include "client/client.h"

void Client::close_connection() {
    mtx_sock.lock();
    if(sock.is_open()) {
        boost::system::error_code ec;
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        if (ec) {
            std::cout << "Error when shutdown socket." << std::endl;
        }
        sock.close(ec);
        if (ec) {
            std::cout << "Error when close socket." << std::endl;
        }
        std::cout << "Close socket." << std::endl;
    }
    mtx_sock.unlock();

//    packets_to_server.clear();
//    std::memset(login, 0, Block::LoginName);
//    std::memset(password, 0, Block::Password);
//    client_id = -1;
//    room_id = 0;
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

void Client::do_connect(work_buf_req_t&&  __buffer) {
    std::cout << "start do_connect()" << std::endl;
    boost::asio::async_connect(sock, eps,
        [this, ptr_buffer = std::move(__buffer)](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) mutable {
           if (!ec) {
               send_login_request(std::move(ptr_buffer));
               std::cout << "success finish do_connect()" << std::endl;
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

void Client::send_login_request(work_buf_req_t&& __buffer) {
    if (__buffer) {
        boost::system::error_code error_code;
    //    bool flag_er_parse = false;

        boost::asio::write(sock, boost::asio::buffer(__buffer.get(), BUF_REQ_LEN), error_code);
        if (error_code) {
            std::cout << "error send_login_request(header)" << std::endl;
            close_connection();
            return ;
        }

        { //---> only for check send
    //        Serialize::Header new_header;
    //        Serialize::Request new_request;
    //        flag_er_parse = new_header.ParseFromArray(__buffer.data(), sizeof(Serialize::Header));
    //        if (flag_er_parse) {
    //            std::cout << "error parse: " << __FILE__ << " " << __LINE__ << std::endl;
    //            close_connection();
    //            return ;
    //        }
    //        flag_er_parse = new_request.ParseFromArray(__buffer.data() + sizeof(Serialize::Header), new_header.length());
    //        if (flag_er_parse) {
    //            std::cout << "error parse: " << __FILE__ << " " << __LINE__ << std::endl;
    //            close_connection();
    //            return ;
    //        }

    //        std::cout << "send: " << "login=" << new_request.input_request().login() << ", password="
    //                  << new_request.input_request().password() << std::endl;
        } //---> end check send

        read_input_response();
    }
    else {
        std::cout << "error: ptr_buffer == nullptr" << std::endl;
    }
}

void Client::read_input_response() {
    boost::system::error_code error_code;
    bool flag_er_parse = false;

    std::vector<uint8_t> __buffer;
    __buffer.resize(sizeof(Serialize::Header));
    boost::asio::read(sock, boost::asio::buffer(__buffer), error_code);
    if (error_code) {
        std::cout << "error when read response(input_res_ptr)" << std::endl;
        close_connection();
        return ;
    }
    Serialize::Header header_response;
    flag_er_parse = header_response.ParseFromArray(__buffer.data(), static_cast<int>(__buffer.size()));
    if (flag_er_parse) {
        std::cout << "error parse: " << __FILE__ << " " << __LINE__ << std::endl;
        close_connection();
        return ;
    }

    if (header_response.command()==static_cast<::google::protobuf::int32>(TypeCommand::RegistrationResponse)
        || header_response.command()==static_cast<::google::protobuf::int32>(TypeCommand::AutorisationResponse))
    {
        // @todo one memory allocation
        __buffer.resize(static_cast<std::size_t>(header_response.length()));
        boost::asio::read(sock, boost::asio::buffer(__buffer),
                          error_code);
        if (error_code) {
            std::cout << "error when read response(data)" << std::endl;
            close_connection();
            return ;
        }
        Serialize::Response response;
        flag_er_parse = response.ParseFromArray(__buffer.data(), static_cast<int>(__buffer.size()));
        if (flag_er_parse) {
            std::cout << "error parse: " << __FILE__ << " " << __LINE__ << std::endl;
            close_connection();
            return ;
        }

        if (header_response.command()==static_cast<::google::protobuf::int32>(TypeCommand::RegistrationResponse)) {
            if (response.reg_response().status() == Serialize::STATUS::FAIL) {
                emit send_input_code(InputCode::BusyRegistr);
                std::cout << "login is busy, id=" << response.reg_response().client_id() << std::endl;
                close_connection();
                return;
            } else {
                emit send_input_code(InputCode::RegistrOK);
            }
        }
        else {
            if (response.input_response().status() == Serialize::STATUS::FAIL) {
                emit send_input_code(InputCode::IncorrectAutor);
                std::cout << "Not found login/password" << std::endl;
                close_connection();
                return;
            }
            else {
                emit send_input_code(InputCode::AutorOK);
            }
        }

        set_login_id(response.input_response().client_id());
//        join_room_request_ptr request = std::make_shared<JoinRoomRequest>(room_id);
//        write(request);

        std::cout << "Success send_login_request()" << std::endl;
        if (!error_code) {
            read_response_header();
        }
    }
    else {
        std::cout << "No innput response from server" << std::endl;
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
