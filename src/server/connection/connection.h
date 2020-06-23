#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <memory>
#include <deque>

#include <server/connection/isubscriber.h>
#include <server/channel/channelsmanager.h>

class Connection : public ISubscriber, public std::enable_shared_from_this<Connection>
{
public:
    Connection(boost::asio::ip::tcp::socket&& socket):
        socket(std::move(socket)), client_id(generate_client_id())
    {

    }

    virtual void start() override {
        read_login_header();
    }

    virtual void send(const Message&) override;

    virtual identifier_t get_client_id() const override {
        return client_id;
    }
    virtual const std::string& get_login() const override { return login; }
private:
    boost::asio::ip::tcp::socket socket;
    Message read_mes;
    std::deque<Message> write_mess;

    const identifier_t client_id;
    std::string login;
private:
    void read_login_header();
    void read_login_body();

    void do_read_header();
    void do_read_body();

    void send_to_client();

    identifier_t generate_client_id() {
        static identifier_t id = 0;
        return ++id;
    }
};

using connection_ptr = std::shared_ptr<Connection>;

#endif // CONNECTION_H
