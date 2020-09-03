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
    BOOST_LOG_TRIVIAL(warning) << "free_connection()";
    ChannelsManager::Instance().leave(client_id, room_id);

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

//    packets_to_client.clear();
    client_id = -1;
    login.clear();
    password.clear();
    busy = false;
    room_id = 0;

    BOOST_LOG_TRIVIAL(warning) << "User exit. Leave from channels. Close socket. Connection saved. ";
}

void Connection::async_read_pb_header() {
    BOOST_LOG_TRIVIAL(info) << "called async_read_pb_header()";

    boost::asio::async_read(socket, boost::asio::buffer(buffer_header),
                                          std::bind(&Connection::do_read_pb_header,
                                                    shared_from_this(),
                                                    std::placeholders::_1,
                                                    std::placeholders::_2));
}

void Connection::do_read_pb_header(boost::system::error_code error, std::size_t nbytes) {
    if (!error) {
         assert(nbytes==Protocol::SIZE_HEADER);

        Serialize::Header new_header;
        bool flag = new_header.ParseFromArray(buffer_header.data(), Protocol::SIZE_HEADER);
        if (flag) {
            async_read_proto_msg(new_header);
        } else {
            BOOST_LOG_TRIVIAL(error) << "parse Header: FAIL";
            async_read_pb_header();
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "error read in async_read_pb_header()";
//        async_read_pb_header();
        free_connection();
    }
}

void Connection::async_read_proto_msg(Serialize::Header header) {
    BOOST_LOG_TRIVIAL(info) << "called read_proto_msg()";
    auto self(shared_from_this());
    buffer_msg.resize(header.length());
    switch (static_cast<TypeCommand>(header.command())) {
        case TypeCommand::AuthorisationRequest:
//            BOOST_LOG_TRIVIAL(info) << "read pb_AuthorisationRequest";
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg.data(), header.length()),
                                    std::bind(&Connection::do_read_pb_input_req,
                                              self,
                                              std::placeholders::_1,
                                              std::placeholders::_2)
                                    );
            break;
        case TypeCommand::RegistrationRequest:
//            BOOST_LOG_TRIVIAL(info) << "read pb_RegistrationRequest";
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg),
                                std::bind(&Connection::read_pb_reg_req,
                                          self,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        break;
        case TypeCommand::JoinRoomRequest:
//            BOOST_LOG_TRIVIAL(info) << "read pb_JoinRoomRequest";
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg),
                                std::bind(&Connection::read_pb_join_room_req,
                                          self,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        break;
        case TypeCommand::EchoRequest:
//           BOOST_LOG_TRIVIAL(info) << "read pb_EchoRequest";
           boost::asio::async_read(socket, boost::asio::buffer(buffer_msg),
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

void Connection::do_read_pb_input_req(boost::system::error_code error, std::size_t) {
    if (!error) {
        BOOST_LOG_TRIVIAL(info) << "read_pb_input_req()";
        Serialize::Request new_request;
        bool parse = new_request.ParseFromArray(buffer_msg.data(), static_cast<int>(buffer_msg.size()));
        if (parse) {
            BOOST_LOG_TRIVIAL(info) << "parse input_request: OK" ;
        } else {
            BOOST_LOG_TRIVIAL(error) << "parse input_request: FAIL" ;
//            std::cout << new_request << std::endl;
        }
        if (new_request.has_input_request()) {
            BOOST_LOG_TRIVIAL(info) << "request include input_request" ;

            // @todo check db
            login = new_request.input_request().login();
            password = new_request.input_request().password();
            client_id = db->check_client(login, password);

            BOOST_LOG_TRIVIAL(info) << "read input_request: login=" << login << ", pwd=" << password << ", client_id=" << client_id;

            auto response_ptr = Protocol::MsgFactory::create_input_response(client_id);
            auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::AutorisationResponse, response_ptr->ByteSizeLong());
            auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));
            add_bin_buf_to_send(std::move(buffer_serialize));

            BOOST_LOG_TRIVIAL(info) << "Authorization successfully completed";
            async_read_pb_header();
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "new_request not have input_request";
        }

    }
    else {
        BOOST_LOG_TRIVIAL(error) << "error read in async_read_pb_input_req()";
    }
}

void Connection::do_write_input_req(boost::system::error_code error, std::size_t nbytes) {
    if (!error) {
        BOOST_LOG_TRIVIAL(info) << "send input_response = " << nbytes << " bytes";
        if (client_id!=-1) {
            BOOST_LOG_TRIVIAL(info) << "Authorization successfully completed";
            async_read_pb_header();
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Authorization failed";
            free_connection();
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error write input_response";
        free_connection();
    }

}

void Connection::read_pb_reg_req(boost::system::error_code error, std::size_t) {
    if (!error && db) {
        BOOST_LOG_TRIVIAL(info) << "read_pb_reg_req()";
        Serialize::Request new_request;
        new_request.ParseFromArray(buffer_msg.data(), static_cast<int>(buffer_msg.size()));
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
        auto buffer_serialize = Protocol::MsgFactory::serialize_response(
                    Protocol::MsgFactory::create_header(TypeCommand::RegistrationResponse, sizeof(Serialize::Response)),
                    Protocol::MsgFactory::create_reg_response(client_id)
                    );

        boost::asio::write(socket, boost::asio::buffer(buffer_serialize));

        if (client_id != -1) {
            BOOST_LOG_TRIVIAL(info) << "Registration successfully completed";
            async_read_pb_header();
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

        Serialize::Request new_request;
        bool flag_parse = new_request.ParseFromArray(buffer_msg.data(), static_cast<int>(buffer_msg.size()));
        if (flag_parse) {
            BOOST_LOG_TRIVIAL(info) << "parse join_room: OK";
        } else {
            BOOST_LOG_TRIVIAL(error) << "parse join_room: FAIL";
        }

        if (new_request.has_join_room_request()) {
            auto new_roomid = new_request.join_room_request().room_id();
            BOOST_LOG_TRIVIAL(info) << "join to roomid=" << new_roomid;

            bool flag = true;
            ChannelsManager::Instance().leave(client_id, room_id);
            flag = ChannelsManager::Instance().join(self, new_roomid, db);

            auto response_ptr = Protocol::MsgFactory::create_join_room_response(room_id, flag);
            auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::JoinRoomResponse, response_ptr->ByteSizeLong());
            auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));
            add_bin_buf_to_send(std::move(buffer_serialize));

            BOOST_LOG_TRIVIAL(info) << "send join_room_response";

        }else {
            BOOST_LOG_TRIVIAL(error) << "request without join_room_request";
        }

        async_read_pb_header();
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
        new_request.ParseFromArray(buffer_msg.data(), static_cast<int>(buffer_msg.size()));

        auto login_sender = new_request.text_request().login();
        auto roomid =  new_request.text_request().room_id();
        auto text = new_request.text_request().text();

        BOOST_LOG_TRIVIAL(info) << "login=" << login_sender << ", roomid=" << roomid << ", text=" << text;

        TextSendData msg{roomid, login_sender, text};

        ChannelsManager::Instance().send_to_channel(msg);
        db->save_text_msg(msg);

        async_read_pb_header();
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "error read_pb_text_req()";
        free_connection();
    }
}

void Connection::send_msg_to_client(const std::string& login,const std::string& text, int room_id) {
    BOOST_LOG_TRIVIAL(info) << "send_msg_to_client";
    auto response_ptr = Protocol::MsgFactory::create_text_response(login, room_id, text);
    BOOST_LOG_TRIVIAL(info) << "login=" << response_ptr->text_response().login() << ", text=" << response_ptr->text_response().text();

    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::EchoResponse, response_ptr->ByteSizeLong());
    auto response_to_send = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

    add_bin_buf_to_send(std::move(response_to_send));
}

void  Connection::add_bin_buf_to_send(std::vector<uint8_t>&& bin_buffer) {
    bool process_write = !bin_buf_to_client.empty();
    bin_buf_to_client.push_back(std::move(bin_buffer));

    if (!process_write) {
        start_send_bin_buffers();
    }
}

void Connection::start_send_bin_buffers() {
//    BOOST_LOG_TRIVIAL(info) << "start_send_bin_buffers()";
    boost::asio::async_write(socket, boost::asio::buffer(bin_buf_to_client.front()),
        [this](boost::system::error_code ec, std::size_t nbytes) {
        if (!ec) {
            std::cout << "write " << nbytes << "bytes" << std::endl;
            bin_buf_to_client.pop_front();
            if (!bin_buf_to_client.empty()) {
                start_send_bin_buffers();
            }
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "error start_send_bin_buffers()";
            free_connection();
        }
    });
}

