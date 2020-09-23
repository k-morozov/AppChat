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
    ChannelsManager::Instance().leave_from_all_channels(client_id);

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

    client_id = -1;
    login = "unused";
    password = "unused";
    busy = false;
    room_id = 0;

    BOOST_LOG_TRIVIAL(warning) << "User exit. Leave from channels. Close socket. Connection saved. ";
}

void Connection::async_read_pb_header() {
    BOOST_LOG_TRIVIAL(info) << "called async_read_pb_header(), thread_id=" << std::this_thread::get_id();

    boost::asio::async_read(socket, boost::asio::buffer(buffer_header),
                                          std::bind(&Connection::do_parse_pb_header,
                                                    shared_from_this(),
                                                    std::placeholders::_1,
                                                    std::placeholders::_2));
}

void Connection::do_parse_pb_header(boost::system::error_code error, std::size_t) {
    if (!error) {
        Serialize::Header new_header;
        bool flag = new_header.ParseFromArray(buffer_header.data(), Protocol::SIZE_HEADER);
        if (flag) {
            buffer_msg.resize(new_header.length());
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg.data(), new_header.length()),
                                    // @todo op lambda, post here?
                                    [this, new_header](boost::system::error_code, std::size_t) {
                                        process_msg(new_header);
                                    });
        } else {
            BOOST_LOG_TRIVIAL(error) << "parse Header: FAIL";
            async_read_pb_header();
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "error read in async_read_pb_header()";
        free_connection();
    }
}

// ************************************* replace Backend ? ******************************************************************
void Connection::process_msg(Serialize::Header header) {
    Serialize::Request new_request;
    bool parse = new_request.ParseFromArray(buffer_msg.data(), static_cast<int>(buffer_msg.size()));
    if (parse) {
        BOOST_LOG_TRIVIAL(info) << "parse input_request: OK" ;
    } else {
        BOOST_LOG_TRIVIAL(error) << "parse input_request: FAIL" ;
    }

    switch (static_cast<TypeCommand>(header.command())) {
        case TypeCommand::AuthorisationRequest:
        {
            boost::asio::post(thread_pool->get_executor() , std::bind(&Connection::do_autorisation,
                                        this,
                                        new_request));
            BOOST_LOG_TRIVIAL(info) << "add do_autorisation in queue" ;
            break;
        }
    case TypeCommand::RegistrationRequest:
    {
        boost::asio::post(thread_pool->get_executor() , std::bind(&Connection::do_registration,
                                    this,
                                    new_request));
        BOOST_LOG_TRIVIAL(info) << "add do_registration in queue" ;
        break;
    }
    case TypeCommand::JoinRoomRequest:
    {
        boost::asio::post(thread_pool->get_executor() , std::bind(&Connection::do_join_room,
                                    this,
                                    new_request));
        BOOST_LOG_TRIVIAL(info) << "add do_join_room in queue" ;
        break;
    }
    case TypeCommand::EchoRequest:
    {
        boost::asio::post(thread_pool->get_executor() , std::bind(&Connection::do_text_msg,
                                    this,
                                    new_request));
        BOOST_LOG_TRIVIAL(info) << "add do_text_msg in queue" ;
        break;
    }
    default:
        BOOST_LOG_TRIVIAL(error) << "read Unknown request";
        break;
    }
    async_read_pb_header();
}

void Connection::do_autorisation(Serialize::Request new_request) {
    BOOST_LOG_TRIVIAL(info) << "called do_autorisation(), thread_id=" << std::this_thread::get_id();
    if (new_request.has_input_request()) {
        BOOST_LOG_TRIVIAL(info) << "request include input_request" ;

        // @todo check db
        login = new_request.input_request().login();
        password = new_request.input_request().password();
        client_id = db->check_client(login, password);

//        BOOST_LOG_TRIVIAL(info) << "read input_request: login=" << login << ", pwd=" << password << ", client_id=" << client_id;
        boost::asio::post(thread_pool->get_executor(),
                          std::bind(&Connection::send_auto_response,
                                    this,
                                    client_id));

        BOOST_LOG_TRIVIAL(info) << "Authorization: " << (client_id!=-1 ? "OK" : "FAIL");
// @todo incorrect inout
//        if (client_id==-1) {
//            free_connection();
//        }
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "new_request not have input_request";
        free_connection();
    }
}

void Connection::do_registration(Serialize::Request new_request) {
    if (new_request.has_register_request()) {
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

//        send_input_response(TypeCommand::RegistrationResponse, client_id);
        boost::asio::post(thread_pool->get_executor(),
                          std::bind(&Connection::send_reg_response,
                                    this,
                                    client_id));

        if (client_id != -1) {
            BOOST_LOG_TRIVIAL(info) << "Registration successfully completed";
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Registration failed";
            // @todo incorrect inout
//            free_connection();
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "new_request not have reg_request";
        free_connection();
    }
}

void Connection::do_join_room(Serialize::Request new_request) {
    BOOST_LOG_TRIVIAL(info) << "called do_join_room(), thread_id=" << std::this_thread::get_id();
    if (new_request.has_join_room_request()) {
        auto self(shared_from_this());
        auto new_roomid = new_request.join_room_request().room_id();
        BOOST_LOG_TRIVIAL(info) << "join to roomid=" << new_roomid;

        bool flag = true;
//        ChannelsManager::Instance().leave(client_id, room_id);
        flag = ChannelsManager::Instance().join(self, new_roomid, db);

        boost::asio::post(thread_pool->get_executor(),
                          std::bind(&Connection::send_join_room_response,
                                    this,
                                    room_id,
                                    flag));
        BOOST_LOG_TRIVIAL(info) << "send join_room_response";

    }else {
        BOOST_LOG_TRIVIAL(error) << "request without join_room_request";
    }
}

void Connection::do_text_msg(Serialize::Request new_request) {
    BOOST_LOG_TRIVIAL(info) << "called do_text_msg(), thread_id=" << std::this_thread::get_id();
    if (new_request.has_text_request()) {
        auto login_sender = new_request.text_request().login();
        auto roomid =  new_request.text_request().room_id();
        auto text = new_request.text_request().text();

        BOOST_LOG_TRIVIAL(info) << "login=" << login_sender << ", roomid=" << roomid << ", text=" << text;

        TextSendData msg{roomid, login_sender, text};

        // @todo ba::post(pool), bd into ChannelsManager
        ChannelsManager::Instance().send_to_channel(msg);
        db->save_text_msg(msg);
    } else {
        BOOST_LOG_TRIVIAL(error) << "request without text_request";
    }
}

void Connection::send_auto_response(identifier_t a_client_id) {
    BOOST_LOG_TRIVIAL(info) << "called send_autorisation_response(), thread_id=" << std::this_thread::get_id();
    auto response_ptr = Protocol::MsgFactory::create_input_response(a_client_id);
    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::AutorisationResponse, response_ptr->ByteSizeLong());
    auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

    boost::asio::async_write(socket, boost::asio::buffer(buffer_serialize),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
                BOOST_LOG_TRIVIAL(info) << "Autorisation response send: OK";
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Autorisation response send: FAIL";
            free_connection();
        }
    });
}

void Connection::send_reg_response(identifier_t a_client_id) {
    BOOST_LOG_TRIVIAL(info) << "called send_reg_response(), thread_id=" << std::this_thread::get_id();
    auto response_ptr = Protocol::MsgFactory::create_reg_response(a_client_id);
    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::RegistrationResponse, response_ptr->ByteSizeLong());
    auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

    boost::asio::async_write(socket, boost::asio::buffer(buffer_serialize),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
                BOOST_LOG_TRIVIAL(info) << "Registration response send: OK";
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Registration response send: FAIL";
            free_connection();
        }
    });
}

void Connection::send_join_room_response(identifier_t a_room_id, bool flag) {
    BOOST_LOG_TRIVIAL(info) << "called send_join_room_response(), thread_id=" << std::this_thread::get_id();
    auto response_ptr = Protocol::MsgFactory::create_join_room_response(a_room_id, flag);
    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::JoinRoomResponse, response_ptr->ByteSizeLong());
    auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

//    add_msg_send_queue(std::move(buffer_serialize));
    boost::asio::async_write(socket, boost::asio::buffer(buffer_serialize),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
                BOOST_LOG_TRIVIAL(info) << "Join_room response send: OK";
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "Join_room response send: FAIL";
            free_connection();
        }
    });
}



// ***********************************************************************************************************************************
void Connection::async_read_pb_msg(Serialize::Header header) {
    BOOST_LOG_TRIVIAL(info) << "called read_proto_msg()";
    auto self(shared_from_this());
    buffer_msg.resize(header.length());
    switch (static_cast<TypeCommand>(header.command())) {
        case TypeCommand::AuthorisationRequest:
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg.data(), header.length()),
                                    std::bind(&Connection::do_read_pb_input_req,
                                              self,
                                              std::placeholders::_1,
                                              std::placeholders::_2)
                                    );
            break;
        case TypeCommand::RegistrationRequest:
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg),
                                std::bind(&Connection::do_read_pb_reg_req,
                                          self,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        break;
        case TypeCommand::JoinRoomRequest:
            boost::asio::async_read(socket, boost::asio::buffer(buffer_msg),
                                std::bind(&Connection::do_read_pb_join_room_req,
                                          self,
                                          std::placeholders::_1,
                                          std::placeholders::_2)
                                );
        break;
        case TypeCommand::EchoRequest:
           boost::asio::async_read(socket, boost::asio::buffer(buffer_msg),
                               std::bind(&Connection::do_read_pb_text_req,
                                         self,
                                         std::placeholders::_1,
                                         std::placeholders::_2)
                               );
        break;
    default:
        BOOST_LOG_TRIVIAL(error) << "read Unknown request";
        break;
    }

    // @todo if ba::post(pool) -> read_header???
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
        }
        if (new_request.has_input_request()) {
            BOOST_LOG_TRIVIAL(info) << "request include input_request" ;

            // @todo check db
            login = new_request.input_request().login();
            password = new_request.input_request().password();
            client_id = db->check_client(login, password);

            BOOST_LOG_TRIVIAL(info) << "read input_request: login=" << login << ", pwd=" << password << ", client_id=" << client_id;

            // @todo create response -> thread_pool
            auto response_ptr = Protocol::MsgFactory::create_input_response(client_id);
            auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::AutorisationResponse, response_ptr->ByteSizeLong());
            auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

            // @todo ba::post(pool) -> write
            add_msg_send_queue(std::move(buffer_serialize));

            BOOST_LOG_TRIVIAL(info) << "Authorization: " << (client_id!=-1 ? "OK" : "FAIL");
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

void Connection::do_read_pb_reg_req(boost::system::error_code error, std::size_t) {
    if (!error && db) {
        BOOST_LOG_TRIVIAL(info) << "called do_read_pb_reg_req()";
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

        auto response_ptr = Protocol::MsgFactory::create_reg_response(client_id);
        auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::RegistrationResponse, response_ptr->ByteSizeLong());
        auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

        // @todo ba::post(pool) -> write
        add_msg_send_queue(std::move(buffer_serialize));

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

void Connection::do_read_pb_join_room_req(boost::system::error_code error, std::size_t) {
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
//            ChannelsManager::Instance().leave(client_id, room_id);
            flag = ChannelsManager::Instance().join(self, new_roomid, db);

            auto response_ptr = Protocol::MsgFactory::create_join_room_response(room_id, flag);
            auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::JoinRoomResponse, response_ptr->ByteSizeLong());
            auto buffer_serialize = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

            // @todo ba::post(pool) -> write
            add_msg_send_queue(std::move(buffer_serialize));

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

void Connection::do_read_pb_text_req(boost::system::error_code error, std::size_t) {
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

        // @todo ba::post(pool), bd into ChannelsManager
        ChannelsManager::Instance().send_to_channel(msg);
        db->save_text_msg(msg);

        async_read_pb_header();
    }
    else {
        BOOST_LOG_TRIVIAL(error) << "error read_pb_text_req()";
        free_connection();
    }
}




// ***********************************************************************************************************************************
void Connection::send_msg_to_client(const std::string& a_login,const std::string& text, int a_room_id) {
    auto response_ptr = Protocol::MsgFactory::create_text_response(a_login, a_room_id, text);
    auto header_ptr = Protocol::MsgFactory::create_header(TypeCommand::EchoResponse, response_ptr->ByteSizeLong());
    auto response_to_send = Protocol::MsgFactory::serialize_response(std::move(header_ptr), std::move(response_ptr));

    add_msg_send_queue(std::move(response_to_send));
}

void  Connection::add_msg_send_queue(std::vector<uint8_t>&& bin_buffer) {
    bool process_write = !send_msgs_queue.empty();
    send_msgs_queue.push(std::move(bin_buffer));
    if (!process_write) {
        sending_msgs_to_client();
    }
}

void Connection::sending_msgs_to_client() {
    boost::asio::async_write(socket, boost::asio::buffer(send_msgs_queue.front()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            send_msgs_queue.pop();
            if (!send_msgs_queue.empty()) {
                sending_msgs_to_client();
            }
        }
        else {
            BOOST_LOG_TRIVIAL(error) << "error start_send_bin_buffers()";
            free_connection();
        }
    });
}

