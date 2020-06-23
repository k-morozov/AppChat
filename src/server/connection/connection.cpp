#include <server/connection/connection.h>

void Connection::send(const Message& message) {
    bool write_in_progress = !write_mess.empty();
    write_mess.push_back(message);
    if (!write_in_progress) {
        send_to_client();
    }
}

void  Connection::read_login_header() {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(read_mes.get_buf_data(), Message::header_size),
        [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                if (read_mes.decode_header()) {
                    read_login_body();
                }
            }
    });
}

void Connection::read_login_body() {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(read_mes.get_buf_id_login(), Message::Settings_zone),
        [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                auto login = read_mes.get_buf_str_login();
                std::cout << "login = " << login << std::endl;
                client_id = generate_client_id();
                std::cout << "new id =" << client_id << std::endl;
                Message num (std::to_string(client_id).c_str());

                boost::asio::write(socket, boost::asio::buffer(&client_id, 4));
                ChannelsManager::Instance().join(self);
                do_read_header();
            }
            else {
//                LOG4CPLUS_INFO(logger, "error async_read when read login-body");
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
//                chat_room.leave(self);
            }
        });
    }

void Connection::do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(socket, boost::asio::buffer(read_mes.get_buf_id_login(), Message::Settings_zone + read_mes.get_body_length()),
        [this, self](boost::system::error_code error, std::size_t) {
            *(read_mes.get_buf_body()+read_mes.get_body_length()) = '\0';
            if (!error) {
                std::cout << read_mes << std::endl;
                ChannelsManager::Instance().send(self, read_mes);

                do_read_header();
            }
            else {
//                    chat_room.leave(self);
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
//                        LOG4CPLUS_INFO(logger, "send: message = " << write_mess.front().get_body());
//                    std::cout << "send: " << write_mess.front().get_buf_body() << std::endl;
                    write_mess.pop_front();
                    if (!write_mess.empty()) {
                        send_to_client();
                    }
                } else {
                    std::cout << "error async_write when write" << std::endl;
//                        LOG4CPLUS_INFO(logger, "error async_write when write");
//                        chat_room.leave(self);
                }
            }
    );
}
