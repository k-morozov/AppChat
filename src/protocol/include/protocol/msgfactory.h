#ifndef MSGFACTORY_H
#define MSGFACTORY_H

#include "command_table.h"
#include "proto/build/messages.pb.h"

namespace Protocol {

using ptr_input_request_t = std::unique_ptr<Serialize::InRequest>;
using ptr_reg_request_t = std::unique_ptr<Serialize::RegRequest>;
using ptr_proto_request_t = std::unique_ptr<Serialize::Request>;
using ptr_header_t = std::unique_ptr<Serialize::Header>;
using ptr_proto_response_t = std::unique_ptr<Serialize::Response>;

constexpr std::size_t SIZE_HEADER = 2*(sizeof(uint32_t)+1) + (sizeof(uint64_t)+1);

class MsgFactory
{
public:
    MsgFactory();

    [[nodiscard]]
    static ptr_proto_request_t create_input_request(const std::string& login, const std::string& password);

    [[nodiscard]]
    static ptr_proto_request_t create_reg_request(const std::string& login, const std::string& password);

    [[nodiscard]]
    static ptr_proto_request_t join_room_request(int room_id);

    [[nodiscard]]
    static ptr_proto_request_t create_text_request(const std::string& login, int room_id, const std::string& text);

    [[nodiscard]]
    static ptr_header_t create_header(TypeCommand command, std::size_t length);

    [[nodiscard]]
    static ptr_proto_response_t create_input_response(int client_id);

    [[nodiscard]]
    static ptr_proto_response_t create_reg_response(int client_id);

    [[nodiscard]]
    static ptr_proto_response_t create_text_response(const std::string& login, int room_id, const std::string& text);

    [[nodiscard]]
    static ptr_proto_response_t create_join_room_response(int new_room_id, bool flag);

    [[nodiscard]]
    static std::vector<uint8_t> serialize_response(ptr_header_t&& header_ptr, ptr_proto_response_t&& response_ptr);

    [[nodiscard]]
    static std::vector<uint8_t> serialize_request(ptr_header_t&& header_ptr, ptr_proto_request_t&& request_ptr);
};

} // end namespace

inline std::ostream& operator<<(std::ostream& stream, const Serialize::Header msg) {
    stream << "header size: "   << msg.ByteSizeLong() << " bytes" << std::endl;
    stream << "version:"        << msg.version() << std::endl;
    stream << "command:"        << msg.command() << std::endl;
//    stream << "time:"           << msg.time() << std::endl;
    stream << "length:"         << msg.length() << std::endl;

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const Serialize::InRequest msg) {
    stream << "read InputRequest: " << msg.ByteSizeLong() << " bytes" << std::endl;
    stream << "login:"              << msg.login() << std::endl;
    stream << "password:"           << msg.password() << std::endl;

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const Serialize::Request msg) {
    if (msg.has_input_request()) {
        stream << msg;
    }

    return stream;
}
#endif // MSGFACTORY_H
