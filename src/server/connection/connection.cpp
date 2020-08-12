#include "server/connection/connection.h"

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
    if (socket.is_open()) {
        socket.close();
    }
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
    request_ptr request = std::make_shared<Request>();

    boost::asio::async_read(socket, boost::asio::buffer(request->get_header(), Block::Header),
                            [this, request](boost::system::error_code error, std::size_t) {
        BOOST_LOG_TRIVIAL(info) << "\n\n";
        if (!error) {
            switch (request->get_type_data()) {
                case TypeCommand::Unknown:
                    BOOST_LOG_TRIVIAL(info) << get_command_str(request->get_type_data()) << ": ";
                    break;
                case TypeCommand::RegistrationRequest:
                    BOOST_LOG_TRIVIAL(info) << get_command_str(request->get_type_data()) << ": ";
                    read_request_body(std::make_shared<RegistrationRequest>(request));
                    break;
                case TypeCommand::RegistrationResponse:
                case TypeCommand::AuthorisationRequest:
                    BOOST_LOG_TRIVIAL(info) << get_command_str(request->get_type_data()) << ": ";
                    read_request_body(std::make_shared<AutorisationRequest>(request));
                    break;
                case TypeCommand::AutorisationResponse:
                case TypeCommand::EchoRequest:
                    BOOST_LOG_TRIVIAL(info) << get_command_str(request->get_type_data()) << ": ";
                    read_request_body(std::make_shared<TextRequest>(request));
                    break;
                case TypeCommand::EchoResponse:
                case TypeCommand::JoinRoomRequest:
                    BOOST_LOG_TRIVIAL(info) << get_command_str(request->get_type_data()) << ": ";
                    read_request_body(std::make_shared<JoinRoomRequest>(request));
                    break;
                case TypeCommand::JoinRoomResponse:
                case TypeCommand::LeaveRoomRequest:
                default:
                    BOOST_LOG_TRIVIAL(info) << get_command_str(request->get_type_data()) << ": ";
                    break;
            }
        } else {
            BOOST_LOG_TRIVIAL(error) << "error read_request_header()";
            free_connection();
        }
    });

}

void Connection::read_request_body(registr_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error && db != nullptr) {
                login = request->get_login();
                password = request->get_password();

                BOOST_LOG_TRIVIAL(info) << "login=" << login << ", pwd=" << password;
                // @todo new generation login_id and check password
                client_id = db->get_loginid(login);
                if (client_id==-1) {
                    client_id = generate_client_id();
                    db->add_logins(login, client_id, password);
                }
                else {
                    BOOST_LOG_TRIVIAL(warning) << "this client was add to db early";
                    client_id = -1;
                }

                input_res_ptr response = std::make_shared<RegistrationResponse>(client_id);
                BOOST_LOG_TRIVIAL(warning) << "RegistrationResponse: vers=" << response->get_protocol_version() << ", command="
                                           << get_command_str(response->get_type_data())
                                           << ", logid=" << response->get_loginid();

                // @todo replace to write
                boost::asio::write(socket, boost::asio::buffer(response->get_header(), Block::Header));
                boost::asio::write(socket, boost::asio::buffer(response->get_data(), response->get_length_data()));

                if (client_id != -1) {
                    BOOST_LOG_TRIVIAL(info) << "Registration successfully completed";
                    read_request_header();
                }
                else {
                    BOOST_LOG_TRIVIAL(error) << "Registration failed";
                    free_connection();
                }
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "error read_request_body(registr)";
                free_connection();
            }
    });
}

void Connection::read_request_body(autor_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error && db != nullptr) {
                login = request->get_login();
                password = request->get_password();

                BOOST_LOG_TRIVIAL(info) << "login=" << request->get_login() << ", pwd=" << request->get_password();
                client_id = db->check_client(login, password);
                input_res_ptr response = std::make_shared<AutorisationResponse>(client_id);

                BOOST_LOG_TRIVIAL(info) << "AutorisationResponse: vers=" << response->get_protocol_version() << ", command="
                                        << get_command_str(response->get_type_data())
                                        << ", logid=" << response->get_loginid();
                if (client_id!=-1) {
                    db->add_logins(login, response->get_loginid(), password);
                }
                boost::asio::write(socket, boost::asio::buffer(response->get_header(), Block::Header));
                boost::asio::write(socket, boost::asio::buffer(response->get_data(), response->get_length_data()));

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
                BOOST_LOG_TRIVIAL(error) << "error read_request_body(autor)";
                free_connection();
            }
    });
}

void Connection::read_request_body(text_request_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_data()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error && db != nullptr) {
                login = request->get_login();
                auto roomid = request->get_roomid();
                auto text = request->get_message();

                BOOST_LOG_TRIVIAL(info) << "login=" << login << ", roomid=" << roomid << ", text=" << text;

                text_response_ptr response = std::make_shared<TextResponse>(login, text, roomid);
                ChannelsManager::Instance().send(response);

                db->save_text_message(request);

                read_request_header();
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "error read_request_body()";
                free_connection();
            }
    });

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
