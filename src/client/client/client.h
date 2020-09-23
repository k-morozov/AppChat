#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <set>
#include <unordered_map>
#include <boost/asio.hpp>
#include "protocol/protocol.h"
#include <QWidget>
#include <QDebug>
#include <QTime>
/**
 * @brief Client class
 * 
 * @details Implement communication with server
 */
class Client: public QObject, public std::enable_shared_from_this<Client> {
   Q_OBJECT

public:
    /**
     * @brief Construct a new Client object
     * @param io boost::asio::io_service
     * @param eps 
     * @param request initial request the server
     */
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& _eps)
        : io_service(io), sock(io), eps(_eps)
    {
        qDebug() << "[" << QTime::currentTime().toString("hh:mm:ss.zzz") << "] " << "create client";

    }

    /**
     * @brief Start connection to the server
     */
    void do_connect(std::vector<uint8_t>&& __buffer);

    void change_room(int room_id);

    void send_msg_to_server(const std::string& text, int room_id);


    auto get_login() const noexcept { return login; }
    void set_login(const std::string& new_login) { login = new_login; }
    auto get_password() const noexcept { return password; }
    void set_password(const std::string& new_password) { password = new_password; }
    auto get_channels() const noexcept {
        std::set<identifier_t> values;
        for(const auto& [channel, history] : channels_history) {
            values.insert(channel);
        }
        return values;
    }
    void set_current_channel(int room_id) noexcept {
        current_room = room_id;
    }

    auto get_current_channel() const noexcept {
        return current_room;
    }

    // @todo const and check limit?
    auto get_history(identifier_t channel_id) {
        return channels_history[channel_id];
    }

    void set_login_id(identifier_t id)   { client_id = id;}

    void close_connection();

    ~Client() {
        std::cout << "Destr client" << std::endl;
        close_connection();
    }


private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;
    std::mutex mtx_sock;
    const boost::asio::ip::tcp::resolver::results_type& eps;

    std::vector<uint8_t> __read_buffer;
    std::array<uint8_t, Protocol::SIZE_HEADER> bin_buffer;
    std::queue<std::vector<uint8_t>> msg_to_server;

    std::string login;
    std::string password;
    identifier_t client_id;
    identifier_t current_room = 0;
    std::unordered_map<identifier_t, std::deque<ClientTextMsg>> channels_history;

private:

    void async_read_pb_header();
    void async_read_pb_msg(Serialize::Header);

    void do_read_pb_header(boost::system::error_code error, std::size_t nbytes);
    void do_read_input_response(boost::system::error_code, std::size_t);
    void do_read_reg_response(boost::system::error_code, std::size_t);
    void do_read_join_room_response(boost::system::error_code, std::size_t);
    void do_read_echo_response(boost::system::error_code, std::size_t);

    void send_login_request(std::vector<uint8_t> && __buffer);

    void add_msg_to_send(std::vector<uint8_t> &&);
    void start_send_msgs();

signals:
    /**
     * @brief 
     * @param from sender's login
     * @param text 
     * @param dt date and time of sending the text
     */
    void send_text(ClientTextMsg);

    /**
     * @brief send input code
     */
    void send_input_code(InputCode);

    void sig_update_channels();
};

#endif // CLIENT_H
