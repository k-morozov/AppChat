#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <set>
#include <deque>
#include <queue>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <protocol/protocol.h>
#include <log/logger.h>

class chat_participant;

using boost::asio::ip::tcp;
using chat_participant_ptr = std::shared_ptr<chat_participant>;
using messege_deque = std::deque<Messages>;

// **********************************************************************************************
class chat_participant {
public:
    virtual ~chat_participant() {};
    virtual void deliver(chat_participant_ptr , const Messages&) = 0;
};

// **********************************************************************************************
class Chat_room  {
public:
    Chat_room(): logger(LOGGER("create Chat_room")) {};

    void join(chat_participant_ptr ptr) {
        LOG4CPLUS_INFO(logger, "join to Chat_room");
        participants.insert(ptr);
        for(const auto& mes:messeges) {
            ptr->deliver(ptr, mes);
        }
    }

    void leave(chat_participant_ptr ptr) {
        LOG4CPLUS_INFO(logger, "leave from Chat_room");
        participants.erase(ptr);
    }

    void deliver(chat_participant_ptr from,const Messages& mes) {
        LOG4CPLUS_INFO(logger, "deliver messege");
        messeges.push_back(mes);
        for(auto& part: participants) {
            if (from.get()!=part.get()) part->deliver(from, mes);
        }
    }

private:
    std::set<chat_participant_ptr> participants;
    messege_deque messeges;
    log4cplus::Logger logger;
};

// **********************************************************************************************
class Chat_session : public chat_participant, public std::enable_shared_from_this<Chat_session> {
public:
    Chat_session(tcp::socket sock, Chat_room &chat_room)
        : sock(std::move(sock)), chat_room(chat_room), logger(LOGGER("Chat_session"))
    {
        LOG4CPLUS_INFO(logger, "new chat session");
    }
    void start() {
        LOG4CPLUS_INFO(logger, "start session");
        chat_room.join(shared_from_this());
        do_read_header();
    }

    virtual void deliver(chat_participant_ptr, const Messages& mes) override {
        bool write_in_progress = !write_mess.empty();
        write_mess.push_back(mes);
        if (!write_in_progress) {
            do_write();
        }
    }
private:
    tcp::socket sock;
    Chat_room &chat_room;
    Messages read_mes;
    messege_deque write_mess;
    log4cplus::Logger logger;
private:
    void do_read_header() {
        LOG4CPLUS_INFO(logger, "read header message" );
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_data(), Messages::header_size),
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
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_body(), read_mes.get_body_length()),
            [this, self](boost::system::error_code error, std::size_t) {
                *(read_mes.get_body()+read_mes.get_body_length()) = '\0';
                if (!error) {
                    LOG4CPLUS_INFO(logger, "message = " << read_mes.get_body());
                    chat_room.deliver(self, read_mes);
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
