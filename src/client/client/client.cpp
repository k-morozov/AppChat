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
}

//void Client::write(const std::string& message) {
//    text_request_ptr text_request = std::make_shared<TextRequest>(login, room_id, message);

//    bool process_write = !packets_to_server.empty();
//    packets_to_server.push_back(text_request);

//    if (!process_write) {
//        send_request_header();
//    }
//}

//void Client::write(text_request_ptr request) {
//    bool process_write = !packets_to_server.empty();
//    packets_to_server.push_back(request);

//    if (!process_write) {
//        send_request_header();
//    }
//}

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

void Client::send_login_request(work_buf_req_t&& __buffer) {
    std::cout << "send_login_request()" << std::endl;
    boost::system::error_code error_code;

    boost::asio::write(sock, boost::asio::buffer(__buffer.get(), BUF_REQ_LEN), error_code);
    if (error_code) {
        std::cout << "error send_login_request(header)" << std::endl;
        close_connection();
        return ;
    }

    read_input_response();
}

void Client::read_input_response() {
    std::cout << "read_input_response()" << std::endl;
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
    std::cout << "full read response" << std::endl;

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

        change_room(room_id);

        std::cout << "Success send_join_room_request()" << std::endl;
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
                    case TypeCommand::EchoRequest:
                        std::cout << "EchoRequest: " << std::endl;
                    break;
                    case TypeCommand::EchoResponse:
                        read_response_data(std::make_shared<TextResponse>(packet));
                        break;

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

void Client::add_msg_to_send(work_buf_req_t&& request_ptr) {
    bool process_write = !msg_to_server.empty();
    msg_to_server.push_back(std::move(request_ptr));

    if (!process_write) {
        start_send_msgs();
    }
}

void Client::start_send_msgs() {
    boost::asio::async_write(sock, boost::asio::buffer(msg_to_server.front().get(), BUF_REQ_LEN),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
                msg_to_server.pop_front();
                if (!msg_to_server.empty()) {
                    start_send_msgs();
                }
        }
        else {
            std::cout << "Error send_request_header()" << std::endl;
            close_connection();
        }
    });
}

void Client::change_room(int new_room_id) {
    auto request_ptr = MsgFactory::join_room_request(new_room_id);
    auto header_ptr = MsgFactory::create_header(TypeCommand::JoinRoomRequest, sizeof(Serialize::Request));
    auto request_to_send = MsgFactory::serialize_request(std::move(header_ptr), std::move(request_ptr));

    add_msg_to_send(std::move(request_to_send));
}

void Client::send_msg_to_server(const std::string& text, int room_id) {
    auto request_ptr = MsgFactory::create_text_request(login, room_id, text);
    auto header_ptr = MsgFactory::create_header(TypeCommand::EchoRequest, sizeof(Serialize::Request));
    auto request_to_send = MsgFactory::serialize_request(std::move(header_ptr), std::move(request_ptr));

    add_msg_to_send(std::move(request_to_send));
}
