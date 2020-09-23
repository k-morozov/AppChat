#include "client/client.h"

void Client::close_connection() {
    mtx_sock.lock();
    if(sock.is_open()) {
        boost::system::error_code ec;
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        if (ec) {
            qWarning() << "Error when shutdown socket.";
        }
        sock.close(ec);
        if (ec) {
            qWarning() << "Error when close socket.";
        }
        qDebug() << "Close socket.";
    }
    mtx_sock.unlock();
}

void Client::do_connect(std::vector<uint8_t>&&  __buffer) {
    qDebug()<< "start do_connect()";
    boost::asio::async_connect(sock, eps,
        [this, ptr_buffer = std::move(__buffer)](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) mutable {
           if (!ec) {
               send_login_request(std::move(ptr_buffer));
               qDebug() << "success finish do_connect()";
           }
           else {
               qWarning() << "error do_connect()";
           }
    });
}

void Client::send_login_request(std::vector<uint8_t>&& __buffer) {
//    qDebug() << "send_login_request()";

    boost::asio::async_write(sock, boost::asio::buffer(__buffer.data(), __buffer.size()),
                                     [this](boost::system::error_code ec, std::size_t nbytes) {
            if (!ec) {
                qDebug() << "send login_request=" << nbytes << " bytes";
                async_read_pb_header();

            } else {
                qWarning() << "error send_login_request(header)";
                close_connection();
            }
    });
}

void Client::async_read_pb_header() {
//    qDebug() << "called async_read_pb_header()";
    boost::asio::async_read(sock, boost::asio::buffer(bin_buffer.data(), Protocol::SIZE_HEADER),
                            std::bind(&Client::do_read_pb_header,
                                      shared_from_this(),
                                      std::placeholders::_1,
                                      std::placeholders::_2));

}

void Client::do_read_pb_header(boost::system::error_code error, std::size_t) {
    if (!error) {
//        qDebug() << "new header read: " << nbytes << " bytes";

        Serialize::Header new_header;
        bool flag = new_header.ParseFromArray(bin_buffer.data(), Protocol::SIZE_HEADER);
        if (flag) {
            qDebug() << "parse header: OK";
        } else {
            qWarning() << "parse header: FAIL";
            return ;
        }
        async_read_pb_msg(new_header);

    } else {
        qWarning() << "error read_pb_request_header()";
        close_connection();
    }
}

void Client::async_read_pb_msg(Serialize::Header new_header) {
    __read_buffer.resize(new_header.length());
    switch (static_cast<TypeCommand>(new_header.command())) {
        case TypeCommand::AutorisationResponse:
            qDebug()<< "AutorisationResponse:"  ;
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_input_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::RegistrationResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_reg_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::EchoRequest:
//            qDebug()<< "EchoRequest: "  ;
        break;
        case TypeCommand::EchoResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_echo_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::JoinRoomResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_join_room_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));

        break;
        default:
            qDebug()<< "Unknown command " << new_header.command();
            break;
    }
}

void Client::do_read_input_response(boost::system::error_code error, std::size_t nbytes) {
    if (!error) {
        qDebug()<< "read responser=" << nbytes << " bytes";

        Serialize::Response response;
        bool flag_parse = response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug() << "parse response: OK";
        } else {
            qWarning() << "parse response: FAIL";
            close_connection();
            return ;
        }

        if (response.has_input_response()) {
            qDebug()<< "response include input_response";
            if (response.input_response().status() == Serialize::STATUS::OK) {
                qDebug()<< "Autorisation response: OK";
                emit send_input_code(InputCode::AutorOK);
            } else {
                emit send_input_code(InputCode::IncorrectAutor);
                qWarning() << "Not found login/password";
                close_connection();
                return;
            }

            set_login_id(response.input_response().client_id());
//            change_room(current_room);

            qDebug()<< "Success send_join_room_request()";
            async_read_pb_header();

        } else {
            qWarning() << "response not include input_response";
        }
    }
    else {
        qWarning() << "error when read response(data)";
        close_connection();
        return ;
    }
}

void Client::do_read_reg_response(boost::system::error_code error, std::size_t ) {
    if (!error) {
//        qDebug()<< "read responser=" << nbytes << " bytes";

        Serialize::Response response;
        bool flag_parse = response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug()<< "parse response: OK";
        } else {
            qWarning() << "parse response: FAIL";
            close_connection();
            return ;
        }

        if (response.has_reg_response()) {
            qDebug()<< "response include reg_response";
            if (response.reg_response().status() == Serialize::STATUS::OK) {
                qDebug()<< "Registration response: OK";
                emit send_input_code(InputCode::RegistrOK);
            } else {
                emit send_input_code(InputCode::BusyRegistr);
                qWarning() << "Registration response: FAIL, busy login";
                close_connection();
                return;
            }

            set_login_id(response.input_response().client_id());
//            change_room(current_room);

            qDebug()<< "Success send_join_room_request()"  ;
            async_read_pb_header();

        } else {
            qWarning() << "response not include reg_response"  ;
        }
    }
    else {
        qWarning() << "error when read response(data)";
        close_connection();
        return ;
    }
}

void Client::do_read_join_room_response(boost::system::error_code error, std::size_t) {
    if (!error) {
//        qDebug()<< "read responser=" << nbytes << " bytes"  ;

        Serialize::Response response;
        bool flag_parse = response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug() << "parse response: OK";
        } else {
            qWarning() << "parse response: FAIL";
            close_connection();
            return ;
        }

        if (response.has_join_room_response()) {
            qDebug() << "response include join_room";
            if (response.join_room_response().status() == Serialize::STATUS::OK) {
                qDebug() << "Response join_room: OK"  ;
                channels_history.try_emplace(response.join_room_response().room_id());
                emit sig_update_channels();

            } else {
                qWarning() << "Response join_room: FAIL";
                close_connection();
                return;
            }
            async_read_pb_header();
        } else {
            qWarning() << "response not include join_room";
        }
    }
    else {
        qDebug()<< "error when read response(data)";
        close_connection();
        return ;
    }
}

void Client::do_read_echo_response(boost::system::error_code error, std::size_t) {
    std::cout << "do_read_echo_response()" << std::endl;
    if (!error) {
        Serialize::Response new_response;
        bool flag_parse = new_response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug()<< "parse echo_response: OK";
        } else {
            qWarning() << "parse echo_response: FAIL";
        }

        // @todo not stable
        ClientTextMsg msg {
            new_response.text_response().login(),
            new_response.text_response().text(),
            new_response.text_response().room_id(),
            DateTime()
        };
        channels_history[msg.channel_id].push_back(msg);
        std::cout << "from server " << msg.author << ": " << msg.text << std::endl;
        send_text(msg);

        async_read_pb_header();
    }
    else {
        qWarning() << "Error read_pb_text_res()";
        close_connection();
    }
}


void Client::add_msg_to_send(std::vector<uint8_t>&& request_ptr) {
    bool process_write = !msg_to_server.empty();
    msg_to_server.push(std::move(request_ptr));

    if (!process_write) {
        start_send_msgs();
    }
}

void Client::start_send_msgs() {
    boost::asio::async_write(sock, boost::asio::buffer(msg_to_server.front()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
                msg_to_server.pop();
                if (!msg_to_server.empty()) {
                    start_send_msgs();
                }
        }
        else {
            qWarning() << "Error send_request_header()";
            close_connection();
        }
    });
}

void Client::change_room(int new_room_id) {
    qDebug() << "called change_room:" << new_room_id;
    auto request_ptr = Protocol::MsgFactory::join_room_request(new_room_id);
    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::JoinRoomRequest, request_ptr->ByteSizeLong());
    auto a_bin_buffer = Protocol::MsgFactory::serialize_request(std::move(header_ptr), std::move(request_ptr));

    add_msg_to_send(std::move(a_bin_buffer));
}

void Client::send_msg_to_server(const std::string& text, int _room_id) {
//    qDebug()<< "send_msg_to_server(): login=" << login << ", text=" << text  ;
    auto request_ptr = Protocol::MsgFactory::create_text_request(login, _room_id, text);
    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::EchoRequest, request_ptr->ByteSizeLong());
    auto a_bin_buffer = Protocol::MsgFactory::serialize_request(std::move(header_ptr), std::move(request_ptr));

    add_msg_to_send(std::move(a_bin_buffer));
}

