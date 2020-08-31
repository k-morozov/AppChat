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
            read_pb_header();
        }
    }
    else {
        std::cout << "No innput response from server" << std::endl;
        close_connection();
    }
}

void Client::read_pb_header() {
    std::cout << "read_pb_header()" << std::endl;
//    std::vector<uint8_t> __read_buffer;
    __read_buffer.resize(sizeof(Serialize::Header));

    boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                            [this](boost::system::error_code error, std::size_t) {
        Serialize::Header new_header;
        new_header.ParseFromArray(__read_buffer.data(), sizeof(Serialize::Header));
        if (!error) {
            read_pb_msg(new_header);
        } else {
            std::cout << "error read_pb_request_header()" << std::endl;
            close_connection();
        }
    });

}

void Client::read_pb_msg(Serialize::Header new_header) {
    switch (static_cast<TypeCommand>(new_header.command())) {
        case TypeCommand::EchoRequest:
            std::cout << "EchoRequest: " << std::endl;
        break;
        case TypeCommand::EchoResponse:
            std::cout << "EchoResponse: " << std::endl;
            read_pb_text_res(new_header);
            break;

        default:
            std::cout << "Unknown command " << new_header.command() << std::endl;
            break;
    }

    read_pb_header();

}

void Client::read_response_data(text_response_ptr) {

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

void Client::send_msg_to_server(const std::string& text, int _room_id) {
    auto request_ptr = MsgFactory::create_text_request(login, _room_id, text);
    auto header_ptr = MsgFactory::create_header(TypeCommand::EchoRequest, sizeof(Serialize::Request));
    auto request_to_send = MsgFactory::serialize_request(std::move(header_ptr), std::move(request_ptr));

    add_msg_to_send(std::move(request_to_send));
}

void Client::read_pb_text_res(Serialize::Header new_header) {
    std::cout << "read_pb_text_res()" << std::endl;
    std::vector<uint8_t> __read_buffer(new_header.length());

    boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
        [this, __read_buffer](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                Serialize::Response new_response;
                new_response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));

                std::cout << ">>>" << new_response.text_response().login()
                          << ": " << new_response.text_response().text() << std::endl;

//                read_pb_header();
            }
            else {
                std::cout << "Error read_pb_text_res()" << std::endl;
                close_connection();
            }
    });
}
