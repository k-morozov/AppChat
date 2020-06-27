#include <client/client.h>

void Client::write(const std::string& message) {
    text_request_ptr text_request = std::make_shared<TextRequest>(login, room_id, message);
//    std::cout << text_request->get_protocol_version()
//              << text_request->get_login() << " " << text_request->get_roomid()
//              << " " << text_request->get_message() << std::endl;

    bool process_write = !packets_to_server.empty();
    packets_to_server.push_back(text_request);

    if (!process_write) {
        start_sending();
    }
}

void Client::do_connect(const boost::asio::ip::tcp::resolver::results_type& eps) {
    auto input_request = logon();

    boost::asio::async_connect(sock, eps,
        [this, input_request](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
           if (!ec) {
               send_login_packet(input_request);
           }
    });
}

input_req_ptr Client::logon() {
    std::cout << "Enter your login: ";
    std::cin.getline(login, Block::LoginName);
    std::cout << "Enter your password: ";
    std::cin.getline(password, Block::Password);
//        std::cout << "Enter room_id: ";
//        std::cin >> room_id;
    std::cout << "************************************" << std::endl;

    return std::make_shared<AutorisationRequest>(login, password);
}

void Client::send_login_packet(packet_ptr packet) {
    boost::system::error_code error_code;

    boost::asio::write(sock, boost::asio::buffer(packet->get_header(),
                                                 Block::Header), error_code);
    boost::asio::write(sock, boost::asio::buffer(packet->get_data(),
                                                 packet->get_length_data()), error_code);
    if (error_code) {
        sock.close();
        std::cout << "error when send login" << std::endl;
        return ;
    }

    input_res_ptr response = std::make_shared<InputResponse>();
    boost::asio::read(sock, boost::asio::buffer(response->get_header(),
                                                Block::Header), error_code);
    if (error_code) {
        sock.close();
        std::cout << "error when read login-id" << std::endl;
        return ;
    }
    if (response->get_type_data()==TypeCommand::RegistrationResponse
        || response->get_type_data()==TypeCommand::AutorisationResponse)
    {
        boost::asio::read(sock, boost::asio::buffer(response->get_data(),
                                                    response->get_length_data()), error_code);
        set_login_id(response->get_loginid());

        std::cout << "logon: OK" << std::endl;
        read_response_header();
    }
    else {
        std::cout << " No response from server" << std::endl;
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
                    break;
                    case TypeCommand::AuthorisationRequest:
                        std::cout << "AuthorisationRequest not been here" << std::endl;
                    break;
                    case TypeCommand::AutorisationResponse:
                        std::cout << "AutorisationResponse" << std::endl;
                        read_response_data(std::make_shared<AutorisationResponse>(packet));
                    break;

                    case TypeCommand::EchoRequest:
                        std::cout << "EchoRequest packet" << std::endl;
                    break;
                    case TypeCommand::EchoResponse:
                        std::cout << "EchoResponse packet" << std::endl;
                        read_response_text_data(std::make_shared<TextResponse>(packet));
                    break;

                    case TypeCommand::JoinRoomResponse:
                    default:
                        std::cout << "Unknown command " << packet->get_protocol_version() << std::endl;
                        break;
                }
            }
            else {
                sock.close();
            }
    });
}

void Client::read_response_data(response_ptr packet) {
    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
                std::cout << "read_response_data" << std::endl;
                read_response_header();
            }
            else {
                sock.close();
            }
    });
}

void Client::read_response_text_data(text_response_ptr packet) {
    boost::asio::async_read(sock, boost::asio::buffer(packet->get_data(), packet->get_length_data()),
        [this, packet](boost::system::error_code error, std::size_t) {
            if (!error) {
                std::cout << packet->get_login() << ": " << packet->get_message() << std::endl;
                read_response_header();
            }
            else {
                sock.close();
            }
    });
}

void Client::start_sending() {
//        std::cout << packets_to_server.front()->get_protocol_version()
//                  << std::dynamic_pointer_cast<TextRequest>(packets_to_server.front())->get_login()
//                  << " " << std::dynamic_pointer_cast<TextRequest>(packets_to_server.front())->get_roomid()
//                  << " " << std::dynamic_pointer_cast<TextRequest>(packets_to_server.front())->get_message()
//        << std::endl;
    boost::asio::async_write(sock, boost::asio::buffer(packets_to_server.front()->get_header(), Block::Header),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
//            std::cout << "header: " << packets_to_server.front()->get_protocol_version() << " "
//                      << static_cast<int32_t>(packets_to_server.front()->get_type_data()) << std::endl;
            send_data();
        }
        else {
            std::cout << "error start_sending" << std::endl;
            sock.close();
        }
    });
}

void Client::send_data() {
    boost::asio::async_write(sock, boost::asio::buffer(packets_to_server.front()->get_data(),
                                                       packets_to_server.front()->get_length_data()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
//            std::cout << "Data: "
//                      << "login=" << std::dynamic_pointer_cast<TextRequest>(packets_to_server.front())->get_login()
//                      << " roomid=" << std::dynamic_pointer_cast<TextRequest>(packets_to_server.front())->get_roomid()
//                      << " text=" << std::dynamic_pointer_cast<TextRequest>(packets_to_server.front())->get_message()
//            << std::endl;
            packets_to_server.pop_front();
            if (!packets_to_server.empty()) start_sending();
        }
        else {
            sock.close();
        }
    });
}
