#ifndef MSGFACTORY_H
#define MSGFACTORY_H

#include "command_table.h"
#include "protocol/messages.pb.h"

using ptr_input_request_t = std::unique_ptr<Serialize::InRequest>;
using ptr_reg_request_t = std::unique_ptr<Serialize::RegRequest>;
using ptr_proto_request_t = std::unique_ptr<Serialize::Request>;
using ptr_header_t =std::unique_ptr<Serialize::Header>;

constexpr uint64_t BUF_REQ_LEN = sizeof(Serialize::Header) + sizeof(Serialize::Request);

using work_buf_req_t = std::unique_ptr<uint8_t[]>;

class MsgFactory
{
public:
    MsgFactory();
    static ptr_proto_request_t create_input_request(const std::string& login, const std::string& password) {
        auto in_request = std::make_unique<Serialize::InRequest>();
        in_request->set_login(login);
        in_request->set_password(password);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_input_request(in_request.release());

        return request;
    }

    static ptr_proto_request_t create_reg_request(const std::string& login, const std::string& password) {
        auto in_request = std::make_unique<Serialize::RegRequest>();
        in_request->set_login(login);
        in_request->set_password(password);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_register_request(in_request.release());

        return request;
    }

    static ptr_header_t create_header(TypeCommand command, int32_t length) {
        auto header = std::make_unique<Serialize::Header>();
        header->set_length(static_cast<google::protobuf::int32>(length));

        header->set_version(1);
        header->set_command(static_cast<google::protobuf::uint64>(command));
        header->set_time(0);

        return header;
    }
};

#endif // MSGFACTORY_H
