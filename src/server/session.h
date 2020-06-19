#ifndef SESSION_H
#define SESSION_H

#include <iostream>
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

using boost::asio::ip::tcp;
using subscriber_ptr = std::shared_ptr<ISubscriber>;
using messege_deque = std::deque<Message>;

// **********************************************************************************************
class ISubscriber {
public:
    virtual ~ISubscriber() {};
    virtual void deliver(subscriber_ptr , const Message&) = 0;
    virtual std::string get_login() const = 0;
};

// **********************************************************************************************
class Chat_room  {
public:
    Chat_room(): logger(LOGGER("Chat_room")) {};

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
        std::cout << mes_leave << std::endl;

        deliver(ptr, Message(mes_leave.data()));

        subscribers.erase(ptr);
    }

    void deliver(subscriber_ptr from, const Message& mes) {
        LOG4CPLUS_INFO(logger, "deliver messege");
//        std::cout << "deliver messege: " << std::endl;

        messeges.push_back(mes);
        for(auto& part: subscribers) {
            if (from.get()!=part.get()) part->deliver(from, mes);
        }
    }

private:
    std::unordered_set<subscriber_ptr> subscribers;
    messege_deque messeges;
    log4cplus::Logger logger;
};

// **********************************************************************************************
class Chat_session : public ISubscriber, public std::enable_shared_from_this<Chat_session> {
public:
    Chat_session(tcp::socket sock, Chat_room &chat_room)
        : sock(std::move(sock)), chat_room(chat_room), logger(LOGGER("Chat_session"))
    {
        LOG4CPLUS_INFO(logger, "new chat session");
    }

    void start() {
        LOG4CPLUS_INFO(logger, "start session");

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
private:
    tcp::socket sock;
    Chat_room &chat_room;
    Message read_mes;
    messege_deque write_mess;
    log4cplus::Logger logger;

    std::string login;
private:

    void read_login_header() {
        LOG4CPLUS_INFO(logger, "read login-header" );
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_data(), Message::header_size),
            [this, self](boost::system::error_code error, std::size_t) {

            if (!error) {
                if (read_mes.decode_header()) {
                    auto size_body = read_mes.get_body_length();
                    LOG4CPLUS_INFO(logger, "size body = " << size_body);

                    read_login_body();
                }
            }
        });
    }
    void read_login_body() {
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_id_body(), Message::login_id_size + read_mes.get_body_length()),
            [this, self](boost::system::error_code error, std::size_t) {
                *(read_mes.get_body()+read_mes.get_body_length()) = '\0';
                if (!error) {
                    LOG4CPLUS_INFO(logger, "login = " << read_mes.get_body());
                    login = read_mes.get_body();
//                    std::cout << "login = " << login << std::endl;

                    int32_t new_id = Database::Instance().get_new_id(login);

                    Message num (std::to_string(new_id).c_str());
                    boost::asio::write(sock,
                                             boost::asio::buffer(&new_id, 4));
                    chat_room.join(self);

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
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_data(), Message::header_size),
                                [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                if (read_mes.decode_header()) {
                    auto size_body = read_mes.get_body_length();
                    LOG4CPLUS_INFO(logger, "size body = " << size_body);
                    do_read_body();
                }
            }
            else {
                LOG4CPLUS_INFO(logger, "error async_read when read header");
                chat_room.leave(self);
            }
        });
    }
    void do_read_body() {
        LOG4CPLUS_INFO(logger, "read body message" );
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_id_body(), Message::login_id_size + read_mes.get_body_length()),
            [this, self](boost::system::error_code error, std::size_t) {
                *(read_mes.get_body()+read_mes.get_body_length()) = '\0';
                if (!error) {
                    LOG4CPLUS_INFO(logger, "message = " << read_mes.get_body());

                    // @todo need opt
                    std::string answer_with_login(Database::Instance().get_name(read_mes.get_id()) + ": " + read_mes.get_body());
                    std::cout << answer_with_login << std::endl;

                    chat_room.deliver(self, Message(answer_with_login.data()));
                    do_read_header();
                }
                else {
                    LOG4CPLUS_INFO(logger, "error async_read when read body");
                    chat_room.leave(self);
                }
        });
    }

    void do_write() {
        LOG4CPLUS_INFO(logger, "write message");
        auto self(shared_from_this());
        boost::asio::async_write(sock,
                                 boost::asio::buffer(write_mess.front().get_data() , write_mess.front().get_mes_length()),
                                 [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    LOG4CPLUS_INFO(logger, "send: message = " << write_mess.front().get_body());
                    write_mess.pop_front();
                    if (!write_mess.empty()) {
                        do_write();
                    }
                } else {
                    LOG4CPLUS_INFO(logger, "error async_write when write");
                    chat_room.leave(self);
                }
        });
    }

};


#endif // SESSION_H
