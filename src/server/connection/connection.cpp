#include "connection/connection.h"

void Connection::reuse(boost::asio::ip::tcp::socket&& _socket) {
    if (busy) {
        BOOST_LOG_TRIVIAL(fatal) << "reuse not free connection";
        free_connection();
    }
    socket = std::move(_socket);
    busy = true;
    BOOST_LOG_TRIVIAL(info) << "reuse connection from " << socket.remote_endpoint().address().to_string()
                            << ":" << socket.remote_endpoint().port();
}

void Connection::free_connection() {
    ChannelsManager::Instance().leave(shared_from_this());

    mtx_sock.lock();
    if(socket.is_open()) {
        boost::system::error_code ec;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        if (ec) {
            BOOST_LOG_TRIVIAL(error) << "Error when shutdown socket.";
        }
        socket.close(ec);
        if (ec) {
            BOOST_LOG_TRIVIAL(error) << "Error when close socket.";
        }
        BOOST_LOG_TRIVIAL(info) << "Close socket.";
    }
    mtx_sock.unlock();

    packets_to_client.clear();
    client_id = -1;
    login.clear();
    password.clear();
    busy = false;

    BOOST_LOG_TRIVIAL(warning) << "User exit. Leave from channels. Close socket. Connection saved. ";
}

void Connection::sendme(text_response_ptr response) {
    bool write_in_progress = !packets_to_client.empty();
    packets_to_client.push_back(response);
    if (!write_in_progress) {
        send_response_header();
    }
}

void Connection::read_request_header() {
    BOOST_LOG_TRIVIAL(info) << "read_request_header()";
    __read_buffer.resize(sizeof(Serialize::Header));

    boost::asio::async_read(socket, boost::asio::buffer(__read_buffer),
                            [this](boost::system::error_code error, std::size_t) {
        Serialize::Header new_header;
        new_header.ParseFromArray(__read_buffer.data(), sizeof(Serialize::Header));
        BOOST_LOG_TRIVIAL(info) << "\n\n";
        if (!error) {
            read_proto_msg(new_header);
        } else {
            BOOST_LOG_TRIVIAL(error) << "error read_request_header()";
            free_connection();
        }
    });

}

void Connection::read_request_body(registr_request_ptr) {
    // delete
}

void Connection::read_request_body(autor_request_ptr) {
    // delete
}

void Connection::read_request_body(text_request_ptr) {
    // delete
}

void Connection::read_request_body(join_room_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                ChannelsManager::Instance().leave(shared_from_this());
                auto new_roomid = request->get_roomid();
                BOOST_LOG_TRIVIAL(info) << "roomid=" << new_roomid;
                ChannelsManager::Instance().join(self, new_roomid, db);

                read_request_header();
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "error read_request_body()";
                free_connection();
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
                    BOOST_LOG_TRIVIAL(error) << "error send_response_header()";
                    free_connection();
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
                    BOOST_LOG_TRIVIAL(error) << "error send_response_data()";
                    free_connection();
                }
            }
    );
}

void Connection::read_proto_msg(Serialize::Header header) {
    BOOST_LOG_TRIVIAL(info) << "read_proto_msg()";
    auto self(shared_from_this());
    __read_buffer.resize(header.length());

    switch (static_cast<TypeCommand>(header.command())) {
        case TypeCommand::AuthorisationRequest:
            BOOST_LOG_TRIVIAL(info) << "read pb_AuthorisationRequest";
            boost::asio::async_read(socket, boost::asio::buffer(__read_buffer),
                                    std::bind(&Connection::read_pb_input_req,
                                              self,
                                              std::placeholders::_1,
                                              std::placeholders::_2)
                                    );
            break;
        case TypeCommand::RegistrationRequest:
            BOOST_LOG_TRIVIAL(info) << "read pb_RegistrationRequest";
            boost::asio::async_read(socket, boost::asio::buffer(__read_buffer),
                                std::bind(&Connection::read_pb_reg_req,
                                          self,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        break;
        case TypeCommand::JoinRoomRequest:
            BOOST_LOG_TRIVIAL(info) << "read pb_JoinRoomRequest";
            boost::asio::async_read(socket, boost::asio::buffer(__read_buffer),
                                std::bind(&Connection::read_pb_join_room_req,
                                          self,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        break;
        case TypeCommand::EchoRequest:
           BOOST_LOG_TRIVIAL(info) << "read pb_EchoRequest";
           boost::asio::async_read(socket, boost::asio::buffer(__read_buffer),
                               std::bind(&Connection::read_pb_text_req,
                                         self,
                                         std::placeholders::_1,
                                         std::placeholders::_2)
                               );
        break;
    default:
        BOOST_LOG_TRIVIAL(error) << "read Unknown request";
        break;
    }
}

void Connection::read_pb_input_req(boost::system::error_code error, std::size_t) {
    if (!error) {
        BOOST_LOG_TRIVIAL(info) << "read_pb_input_req()";
        Serialize::Request new_request;
        new_request.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));

        login = new_request.input_request().login();
        password = new_request.input_request().password();
        BOOST_LOG_TRIVIAL(info) << "read from input_request: login=" << login << ", pwd=" << password;
        client_id = db->check_client(login, password);

        auto buffer_serialize = MsgFactory::serialize_response(
                    MsgFactory::create_header(TypeCommand::AutorisationResponse, sizeof(Serialize::Response)),
                    MsgFactory::create_input_response(client_id)
                    );
        boost::asio::write(socket, boost::asio::buffer(buffer_serialize.get(), BUF_RES_LEN));
        if (client_id!=-1) {
            BOOST_LOG_TRIVIAL(info) << "Authorization successfully completed";
            read_request_header();
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Authorization failed";
            free_connection();
        }
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "error read_pb_input_req()";
    }
}

void Connection::read_pb_reg_req(boost::system::error_code error, std::size_t) {
    if (!error && db) {
        BOOST_LOG_TRIVIAL(info) << "read_pb_reg_req()";
        Serialize::Request new_request;
        new_request.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        login = new_request.register_request().login();
        password = new_request.register_request().password();

        BOOST_LOG_TRIVIAL(info) << "read from reg_request: login=" << login << ", pwd=" << password;

        client_id = db->get_loginid(login);
        if (client_id == -1) {
            client_id = generate_client_id();
            db->add_logins(login, client_id, password);
        }
        else {
            BOOST_LOG_TRIVIAL(warning) << "this client was add to db early";
            client_id = -1;
        }
        auto buffer_serialize = MsgFactory::serialize_response(
                    MsgFactory::create_header(TypeCommand::RegistrationResponse, sizeof(Serialize::Response)),
                    MsgFactory::create_reg_response(client_id)
                    );

        boost::asio::write(socket, boost::asio::buffer(buffer_serialize.get(), BUF_RES_LEN));

        if (client_id != -1) {
            BOOST_LOG_TRIVIAL(info) << "Registration successfully completed";
            read_request_header();
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Registration failed";
            free_connection();
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "error read_request_body(registr)";
        free_connection();
    }
}

void Connection::read_pb_join_room_req(boost::system::error_code error, std::size_t) {
    if (!error) {
        auto self(shared_from_this());
        BOOST_LOG_TRIVIAL(info) << "read_pb_join_room_req()";
        ChannelsManager::Instance().leave(shared_from_this());

        Serialize::Request new_request;
        new_request.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));

        auto new_roomid = new_request.join_room_request().room_id();
        BOOST_LOG_TRIVIAL(info) << "new roomid=" << new_roomid;
        ChannelsManager::Instance().join(self, new_roomid, db);

        read_request_header();
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "error read_pb_join_room_req()";
        free_connection();
    }
}

void Connection::read_pb_text_req(boost::system::error_code error, std::size_t) {
    if (!error && db) {
        auto self(shared_from_this());
        BOOST_LOG_TRIVIAL(info) << "read_pb_text_req()";

        Serialize::Request new_request;
        new_request.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));

        login = new_request.text_request().login();
        auto roomid =  new_request.text_request().room_id();
        auto text = new_request.text_request().text();

        BOOST_LOG_TRIVIAL(info) << "login=" << login << ", roomid=" << roomid << ", text=" << text;

        auto text_response = MsgFactory::create_text_response(login, roomid, text);

//        start here
//        ChannelsManager::Instance().send(response);
//        db->save_text_message(request);

        read_request_header();
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "error read_pb_text_req()";
        free_connection();
    }
}

