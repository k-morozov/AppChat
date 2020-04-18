#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <set>
#include <deque>
#include <queue>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class chat_participant {
public:
    virtual ~chat_participant() {};
    virtual void deliver(std::string) = 0;
};

using chat_participant_ptr = std::shared_ptr<chat_participant>;
using messege_deque = std::deque<std::string>;

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

    void deliver(std::string mes) {
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
        do_read();
    }

    virtual void deliver(std::string mes) override {
        bool write_in_progress = !write_mess.empty();
        write_mess.push_back(mes);
        if (!write_in_progress) {
            do_write();
        }
    }
private:
    tcp::socket sock;
    Chat_room &chat_room;
    char read_mes[256];
    messege_deque write_mess;

private:
    void do_read() {
        auto self(shared_from_this());
        boost::asio::async_read(sock, boost::asio::buffer(read_mes, 4),
                                [this, self](boost::system::error_code error, std::size_t) {
            if (!error) {
                chat_room.deliver(read_mes);
                do_read();
            }
            else {
                chat_room.leave(self);
            }
        });
    }

    void do_write() {
        auto self(shared_from_this());
        boost::asio::async_write(sock,
                                 boost::asio::buffer(write_mess.front().data(), write_mess.front().length()),
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
