#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <deque>
#include <queue>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <protocol/protocol.h>
#include <log/logger.h>
#include <server/database.h>

class ISubscriber;
class Chat_room;
class Chat_session;

using boost::asio::ip::tcp;
using subscriber_ptr = std::shared_ptr<ISubscriber>;
using messege_deque = std::deque<Message>;
using room_ptr = std::shared_ptr<Chat_room>;

static constexpr int32_t Default_room_id = 0;

// **********************************************************************************************
class ISubscriber {
public:
    virtual ~ISubscriber() {};
    virtual void deliver(subscriber_ptr , const Message&) = 0;
    virtual std::string get_login() const = 0;
    virtual int32_t get_login_id() const = 0;
};

// **********************************************************************************************
class Chat_room  {
public:
    Chat_room(int32_t id = 0): room_id(id), logger(LOGGER("Chat_room")) {};

    void join(subscriber_ptr ptr) {
        LOG4CPLUS_INFO(logger, "New subscriber: " << ptr->get_login());
        std::cout << "New subscriber: " << ptr->get_login() << std::endl;

        subscribers.insert(ptr);
        for(const auto& mes:messeges) {
            ptr->deliver(ptr, mes);
        }
    }

    void leave(subscriber_ptr ptr) {
        LOG4CPLUS_INFO(logger, "leave from Chat_room" << ptr->get_login());

        std::string mes_leave("leave from Chat_room: " + ptr->get_login());
        Message mes(mes_leave);
        mes.set_login("server");

        deliver(ptr, mes);
        subscribers.erase(ptr);
    }

    void deliver(subscriber_ptr from, const Message& mes) {
        LOG4CPLUS_INFO(logger, "send messege");
//        std::cout << "deliver messege: " << std::endl;

        messeges.push_back(mes);
        std::cout << "send: " << "login=" <<mes.get_buf_str_login() << ", login_id=" <<
                  mes.get_login_id() <<
                     ", room_id=" << mes.get_room_id() <<
                     ", message=" << mes.get_buf_body()
                  << std::endl;

        for(auto& part: subscribers) {
            if (mes.get_login_id() != part->get_login_id()) {
                std::cout << mes.get_login_id() << " and " << part->get_login_id() << std::endl;
                part->deliver(from, mes);
            }
        }
    }

    auto get_room_id() const { return room_id; }
private:
    int32_t room_id;
    std::unordered_set<subscriber_ptr> subscribers;
    messege_deque messeges;
    log4cplus::Logger logger;
};

// **********************************************************************************************
class Chat_session : public ISubscriber, public std::enable_shared_from_this<Chat_session> {
public:
    Chat_session(tcp::socket _sock, room_ptr chat_room)
        : sock(std::move(_sock)), chat_room(chat_room), logger(LOGGER("Chat_session"))
    {
        LOG4CPLUS_INFO(logger, "new chat session");
        ip_client = sock.remote_endpoint().address();
        port_client = sock.remote_endpoint().port();
        login_id = -1;
    }

    void start() {
        LOG4CPLUS_INFO(logger, "start session");
        std::cout << "new connect from ip=" << ip_client.to_string() <<
                  ", port=" << port_client << std::endl;

        read_login_header();
    }

    virtual void deliver(subscriber_ptr, const Message& mes) override {
        bool write_in_progress = !write_mess.empty();
        write_mess.push_back(mes);
        if (!write_in_progress) {
            do_write();
        }
    }
    virtual std::string get_login() const override {
        return login;
    }

    virtual int32_t get_login_id() const override {
        return login_id;
    }


    virtual ~Chat_session() {
        LOG4CPLUS_INFO(logger, "end session");
        std::cout << "close session from ip=" << ip_client.to_string() <<
                      ", port=" << port_client << std::endl;
    }
private:
    tcp::socket sock;
    boost::asio::ip::address ip_client;
    unsigned short port_client;

    room_ptr chat_room;
    Message read_mes;
    messege_deque write_mess;
    log4cplus::Logger logger;

    std::string login;
    int32_t login_id;
private:

    void read_login_header() {
        LOG4CPLUS_INFO(logger, "read login-header" );
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_buf_data(), Message::header_size),
            [this, self](boost::system::error_code error, std::size_t) {

            if (!error) {
                if (read_mes.decode_header()) {
                    auto size_body = read_mes.get_body_length();
                    LOG4CPLUS_INFO(logger, "size body = " << size_body);

                    read_login_body();
                }
            }
            else {
                LOG4CPLUS_INFO(logger, "error async_read when read login-body");
                std::cout << "Error read header-login" << std::endl;
                sock.close();
            }
        });
    }
    void read_login_body() {
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_buf_id_login(), Message::Settings_zone),
            [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    LOG4CPLUS_INFO(logger, "login = " << read_mes.get_buf_body());
                    login = read_mes.get_buf_str_login();
//                    std::cout << "login = " << login << std::endl;

                    login_id = Database::Instance().get_new_id(login);

                    Message num (std::to_string(login_id).c_str());
                    boost::asio::write(sock, boost::asio::buffer(&login_id, 4));
                    chat_room->join(self);
                    do_read_header();
                }
                else {
                    LOG4CPLUS_INFO(logger, "error async_read when read login-body");
                    sock.close();
                }
        });
    }

    void do_read_header() {
        LOG4CPLUS_INFO(logger, "read header message" );
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_buf_data(), Message::header_size),
                                [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                if (read_mes.decode_header()) {
//                    auto size_body = read_mes.get_body_length();
//                    LOG4CPLUS_INFO(logger, "size body = " << size_body);
                    do_read_body();
                }
            }
            else {
                LOG4CPLUS_INFO(logger, "error async_read when read header");
                std::cout << "Error read header" << std::endl;
                chat_room->leave(self);
            }
        });
    }
    void do_read_body() {
        LOG4CPLUS_INFO(logger, "read body message" );
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_buf_id_login(), Message::Settings_zone + read_mes.get_body_length()),
            [this, self](boost::system::error_code error, std::size_t) {
//                std::cout << "room = " << read_mes.get_room_id() << std::endl;
                if (read_mes.get_room_id() != chat_room->get_room_id()) {
                    std::cout << "Change room from " << chat_room->get_room_id() << " to "
                              << read_mes.get_room_id() ;
                    // change room?

                }

                *(read_mes.get_buf_body()+read_mes.get_body_length()) = '\0';
                if (!error) {
                    LOG4CPLUS_INFO(logger, "message = " << read_mes.get_buf_body());

                    chat_room->deliver(self, read_mes);
                    do_read_header();
                }
                else {
                    LOG4CPLUS_INFO(logger, "error async_read when read body");
                    chat_room->leave(self);
                }
        });
    }

    void do_write() {
        LOG4CPLUS_INFO(logger, "write message");
        auto self(shared_from_this());
        boost::asio::async_write(sock,
                                 boost::asio::buffer(write_mess.front().get_buf_data() , write_mess.front().get_mes_length()),
                                 [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
//                    LOG4CPLUS_INFO(logger, "send: message = " << write_mess.front().get_buf_body());
//                    std::cout << "send: " << write_mess.front().get_buf_body() << std::endl;
                    write_mess.pop_front();
                    if (!write_mess.empty()) {
                        do_write();
                    }
                } else {
                    LOG4CPLUS_INFO(logger, "error async_write when write");
                    chat_room->leave(self);
                }
        });
    }

};


#endif // SESSION_H
