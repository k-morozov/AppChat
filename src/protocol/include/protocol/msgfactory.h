#ifndef MSGFACTORY_H
#define MSGFACTORY_H

#include "command_table.h"
#include "protocol/messages.pb.h"

namespace Protocol {

using ptr_input_request_t = std::unique_ptr<Serialize::InRequest>;
using ptr_reg_request_t = std::unique_ptr<Serialize::RegRequest>;
using ptr_proto_request_t = std::unique_ptr<Serialize::Request>;
using ptr_header_t = std::unique_ptr<Serialize::Header>;
using ptr_proto_response_t = std::unique_ptr<Serialize::Response>;

constexpr uint64_t BUF_REQ_LEN = sizeof(Serialize::Header) + sizeof(Serialize::Request);
constexpr uint64_t BUF_RES_LEN = sizeof(Serialize::Header) + sizeof(Serialize::Response);

using work_buf_req_t = std::unique_ptr<uint8_t[]>;
using work_buf_res_t = std::unique_ptr<uint8_t[]>;

constexpr std::size_t SIZE_HEADER = 3*(sizeof(int32_t)+1);

class MsgFactory
{
public:
    MsgFactory();

    static ptr_proto_request_t create_input_request(const std::string& login, const std::string& password);

    static ptr_proto_request_t create_reg_request(const std::string& login, const std::string& password);

    static ptr_proto_request_t join_room_request(int room_id);

    static ptr_proto_request_t create_text_request(const std::string& login, int room_id, const std::string& text);

    static ptr_header_t create_header(TypeCommand command, int32_t length);

    static ptr_proto_response_t create_input_response(int client_id);

    static ptr_proto_response_t create_reg_response(int client_id);

    static ptr_proto_response_t create_text_response(const std::string& login, int room_id, const std::string& text);

    static ptr_proto_response_t create_join_room_response(int new_room_id, bool flag);

    static std::vector<uint8_t> serialize_response(ptr_header_t&& header_ptr, ptr_proto_response_t&& response_ptr);

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
