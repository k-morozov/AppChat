#include <client/client.h>

void Client::write(Message mes) {
    mes.set_id(client_id);
    bool process_write = !sending_message.empty();
    sending_message.push_back(mes);

    if (!process_write) {
        do_write();
    }
}

void Client::do_connect(const boost::asio::ip::tcp::resolver::results_type& eps) {
    boost::asio::async_connect(sock, eps,
        [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) {
           if (!ec) {
               send_login(Message(login));
           }
    });
}

void Client::do_send_login(const Message& message) {
    boost::system::error_code error_code;
    boost::asio::write(sock, boost::asio::buffer(message.get_data(), message.get_mes_length()), error_code);
    std::cout << "do send login: " << message.get_body() << std::endl;
    if (error_code) {
        sock.close();
        std::cout << "error when send login" << std::endl;
        return ;
    }
    int32_t id;
    boost::asio::read(sock, boost::asio::buffer(&id, 4), error_code);
    if (error_code) {
        sock.close();
        std::cout << "error when read login-id" << std::endl;
        return ;
    }
    this->set_id(id);
    std::cout << "my id = " << id << std::endl;
    flag_logon = true;

    do_read_header();
}

void Client::do_read_header() {
    boost::asio::async_read(sock, boost::asio::buffer(receiving_message.get_data(), Message::header_size),
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
    boost::asio::async_read(sock, boost::asio::buffer(receiving_message.get_id_body(), Message::login_id_size + size_body),
        [this](boost::system::error_code error, std::size_t) {
            if (!error) {
                *(receiving_message.get_body()+receiving_message.get_body_length()) = '\0';
                std::cout << receiving_message.get_body() << std::endl;
                do_read_header();
            }
            else {
                sock.close();
            }
    });
}

void Client::do_write() {
    boost::asio::async_write(sock, boost::asio::buffer(sending_message.front().get_data(), sending_message.front().get_mes_length()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
//                std::cout << "send: " << receiving_message.get_body() << std::endl;
            sending_message.pop_front();
            if (!sending_message.empty()) do_write();
        }
        else {
            sock.close();
        }
    });
}
