#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <set>
#include <deque>
#include <queue>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "../protocol/protocol.h"

using boost::asio::ip::tcp;

class chat_participant {
public:
    virtual ~chat_participant() {};
    virtual void deliver(const Messages&) = 0;
};

using chat_participant_ptr = std::shared_ptr<chat_participant>;
using messege_deque = std::deque<Messages>;

class Chat_room  {
public:
    Chat_room() = default;

    void join(chat_participant_ptr ptr) {
        std::cout << "join to room" << std::endl;
        participants.insert(ptr);
        for(auto mes:messeges) {
            ptr->deliver(mes);
        }
    }

    void leave(chat_participant_ptr ptr) {
        participants.erase(ptr);
    }

    void deliver(const Messages& mes) {
        for(auto& part: participants) {
            part->deliver(mes);
        }
    }

private:
    std::set<chat_participant_ptr> participants;
    messege_deque messeges;
};

class Chat_session : public chat_participant, public std::enable_shared_from_this<Chat_session> {
public:
    Chat_session(tcp::socket sock, Chat_room &chat_room)
        : sock(std::move(sock)), chat_room(chat_room)
    {
        std::cout << "new chat session" << std::endl;
    }
    void start() {
        chat_room.join(shared_from_this());
        do_read_header();
    }

    virtual void deliver(const Messages& mes) override {
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

private:
    void do_read_header() {
//        std::cout << "do_read_header" << std::endl;
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_data(), Messages::header_size),
                                [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                auto size_body = read_mes.get_length()-Messages::header_size;
                std::cout << "size body = " << size_body << std::endl;
                do_read_body(size_body);
            }
            else {
                chat_room.leave(self);
            }
        });
    }
    void do_read_body(std::size_t size_body) {
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes.get_body(), size_body),
            [this, self](boost::system::error_code error, std::size_t) {
                std::cout << "body = " << read_mes.get_body() << std::endl;
                if (!error) {
                    chat_room.deliver(read_mes);
                    do_read_header();
                }
                else {
                    chat_room.leave(self);
                }
        });
    }
    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(sock,
                                 boost::asio::buffer(write_mess.front().get_data() , write_mess.front().get_length()),
                                 [this, self](boost::system::error_code error, std::size_t) {
                if (!error) {
                    write_mess.pop_front();
                    if (!write_mess.empty()) {
                        do_write();
                    }
                } else {
                    chat_room.leave(self);
                }
        });
    }

};


#endif // SESSION_H
