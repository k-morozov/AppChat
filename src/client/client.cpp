#include <client/client.h>

void Client::write(Message mes) {
    mes.set_login_id(client_id);
    mes.set_login(login);
//    std::cout << "write: " << mes.get_login_id() << std::endl;
    mes.set_room_id(room_id);

    bool process_write = !sending_message.empty();
    sending_message.push_back(mes);

    if (!process_write) {
        do_write();
    }
}

void Client::do_connect(const boost::asio::ip::tcp::resolver::results_type& eps) {
    auto input_request = logon();
//    std::cout << input_request->get_protocol_version() << " " << input_request->get_type_data() << std::endl;
//    std::cout << input_request->get_login() << " and " << input_request->get_password() << std::endl;
    boost::asio::async_connect(sock, eps,
        [this, input_request](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
           if (!ec) {
               send_input_request(input_request);
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

void Client::send_input_request(request_ptr input_request) {
    boost::system::error_code error_code;

    boost::asio::write(sock, boost::asio::buffer(input_request->get_header(),
                                                 Block::Header +input_request->get_length_request()), error_code);
    if (error_code) {
        sock.close();
        std::cout << "error when send login" << std::endl;
        return ;
    }

    input_res_ptr response = std::make_shared<AutorisationResponse>();
    boost::asio::read(sock, boost::asio::buffer(response->get_header(),
                                                Block::Header), error_code);
    if (error_code) {
        sock.close();
        std::cout << "error when read login-id" << std::endl;
        return ;
    }
    if (response->get_type_data()==TypeCommand::RegistrationResponse
        || response->get_type_data()==TypeCommand::AuthorisationResponse)
    {
        boost::asio::read(sock, boost::asio::buffer(response->get_data(),
                                                    response->get_length_response()), error_code);
        set_login_id(response->get_loginid());
        std::cout << "loginID=" << response->get_loginid() << std::endl;
    //    do_read_header();
    }

}

void Client::do_read_header() {
    boost::asio::async_read(sock, boost::asio::buffer(receiving_message.get_buf_data(), Message::header_size),
        [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                if (receiving_message.decode_header())
                {
                    auto size_body = receiving_message.get_body_length();
                    do_read_body(size_body);
                }
            }
            else {
                sock.close();
            }
    });
}

void Client::do_read_body(std::size_t size_body) {
    boost::asio::async_read(sock, boost::asio::buffer(receiving_message.get_buf_id_login(), Message::Settings_zone + size_body),
        [this](boost::system::error_code error, std::size_t) {
            if (!error) {
                *(receiving_message.get_buf_body()+receiving_message.get_body_length()) = '\0';
                std::cout << receiving_message.get_buf_str_login() << ": "
                          << receiving_message.get_buf_body() << std::endl;
                do_read_header();
            }
            else {
                sock.close();
            }
    });
}

void Client::do_write() {
    boost::asio::async_write(sock, boost::asio::buffer(sending_message.front().get_buf_data(), sending_message.front().get_mes_length()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
//            std::cout << sending_message.front() << std::endl;
//            std::cout << "mes with room_id=" << sending_message.front().get_room_id() << std::endl;
            sending_message.pop_front();
            if (!sending_message.empty()) do_write();
        }
        else {
            sock.close();
        }
    });
}
