#include <server/connection/connection.h>

void Connection::send(const Message& message) {
    bool write_in_progress = !write_mess.empty();
    write_mess.push_back(message);
    if (!write_in_progress) {
        send_to_client();
    }
}

void Connection::read_request_header() {
    boost::system::error_code error;
    request_ptr request = std::make_shared<Request>();
    boost::asio::read(socket, boost::asio::buffer(request->get_header(), Block::Header), error);

    if (!error) {

        switch (request->get_type_data()) {
            case TypeCommand::Unknown:
            case TypeCommand::RegistrationRequest:
                read_input_request_body(std::make_shared<RegistrationRequest>(request));
                break;
            case TypeCommand::RegistrationResponse:
            case TypeCommand::AuthorisationRequest:
                read_input_request_body(std::make_shared<AutorisationRequest>(request));
                break;
            case TypeCommand::AuthorisationResponse:
            case TypeCommand::EchoRequest:
            case TypeCommand::EchoResponse:
            case TypeCommand::JoinRoomRequest:
            case TypeCommand::JoinRoomResponse:
            case TypeCommand::LeaveRoomRequest:
            default:
                break;
        }

    } else {
        std::cout << "error read_request_header" << std::endl;
        socket.close();
    }
}

void Connection::read_input_request_body(input_req_ptr request) {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(request->get_data(), request->get_length_request()),
        [this, self, request](boost::system::error_code error, std::size_t) {
            if (!error) {
                login = request->get_login();
                password = request->get_password();
                std::cout << request->get_login() << " " << request->get_password() << std::endl;

                input_res_ptr response = std::make_shared<AutorisationResponse>(client_id);

                boost::asio::write(socket, boost::asio::buffer(response->get_header(), Block::Header));
                boost::asio::write(socket, boost::asio::buffer(response->get_data(), response->get_length_response()));

                ChannelsManager::Instance().join(self, 0);
//                do_read_header();
            }
            else {
                socket.close();
            }
    });
}

void Connection::do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(read_mes.get_buf_data(), Message::header_size),
        [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                if (read_mes.decode_header()) {
                    do_read_body();
                }
            }
            else {
//                std::cout << "need leave" << std::endl;
                ChannelsManager::Instance().leave(self);
            }
        });
    }

void Connection::do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(read_mes.get_buf_id_login(), Message::Settings_zone + read_mes.get_body_length()),
        [this, self](boost::system::error_code error, std::size_t) {
            *(read_mes.get_buf_body()+read_mes.get_body_length()) = '\0';
            if (!error) {
                if (read_mes.get_body_length()) {
                    std::cout << read_mes << std::endl;
                    ChannelsManager::Instance().send(read_mes);
                }
                do_read_header();
            }
            else {
                ChannelsManager::Instance().leave(self);
            }
        }
    );
}

void Connection::send_to_client() {
    auto self(shared_from_this());
    boost::asio::async_write(socket,
        boost::asio::buffer(write_mess.front().get_buf_data() , write_mess.front().get_mes_length()),
            [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    write_mess.pop_front();
                    if (!write_mess.empty()) {
                        send_to_client();
                    }
                } else {
                    std::cout << "error async_write when write" << std::endl;
                    ChannelsManager::Instance().leave(self);
                }
            }
    );
}
