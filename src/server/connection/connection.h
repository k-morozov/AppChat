#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <memory>
#include <deque>

#include <server/connection/isubscriber.h>
#include <server/channel/channels_manager.h>
#include <protocol/protocol.h>

class Connection : public ISubscriber, public std::enable_shared_from_this<Connection>
{
public:
    Connection(boost::asio::ip::tcp::socket&& _socket):
        socket(std::move(_socket)), client_id(generate_client_id())
    {
        std::cout << "new connection from " << socket.remote_endpoint() .address().to_string()
                  << ":" << socket.remote_endpoint() .port() << std::endl;
    }

    virtual void start() override {
        read_request_header();
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
    std::string password;

private:
    void read_request_header();
    void read_input_request_body(input_req_ptr);

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
