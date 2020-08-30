#ifndef MSGFACTORY_H
#define MSGFACTORY_H

#include "command_table.h"
#include "protocol/messages.pb.h"

using ptr_input_request_t = std::unique_ptr<Serialize::InRequest>;
using ptr_reg_request_t = std::unique_ptr<Serialize::RegRequest>;
using ptr_proto_request_t = std::unique_ptr<Serialize::Request>;
using ptr_header_t = std::unique_ptr<Serialize::Header>;
using ptr_proto_response_t = std::unique_ptr<Serialize::Response>;

constexpr uint64_t BUF_REQ_LEN = sizeof(Serialize::Header) + sizeof(Serialize::Request);
constexpr uint64_t BUF_RES_LEN = sizeof(Serialize::Header) + sizeof(Serialize::Response);

using work_buf_req_t = std::unique_ptr<uint8_t[]>;
using work_buf_res_t = std::unique_ptr<uint8_t[]>;

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

    static ptr_proto_request_t join_room_request(int room_id) {
        auto jr_request = std::make_unique<Serialize::JoinRoomRequest>();
        jr_request->set_room_id(room_id);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_join_room_request(jr_request.release());

        return request;
    }

    static ptr_proto_request_t create_text_request(const std::string& login, int room_id, const std::string& text) {
        auto text_request = std::make_unique<Serialize::TextRequest>();
        text_request->set_login(login);
        text_request->set_room_id(room_id);
        text_request->set_text(text);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_text_request(text_request.release());

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

    static ptr_proto_response_t create_input_response(int client_id) {
        auto in_response = std::make_unique<Serialize::InputResponse>();
        in_response->set_client_id(client_id);
        in_response->set_msg_id(0);
        in_response->set_chat_id(0);
        if (client_id!=-1) {
            in_response->set_status(Serialize::STATUS::OK);
        }
        else {
            in_response->set_status(Serialize::STATUS::FAIL);
        }
        auto response = std::make_unique<Serialize::Response>();
        response->set_allocated_input_response(in_response.release());

        return response;
    }

    static ptr_proto_response_t create_reg_response(int client_id) {
        auto reg_response = std::make_unique<Serialize::RegResponse>();
        reg_response->set_client_id(client_id);
        reg_response->set_msg_id(0);
        reg_response->set_chat_id(0);
        if (client_id!=-1) {
            reg_response->set_status(Serialize::STATUS::OK);
        }
        else {
            reg_response->set_status(Serialize::STATUS::FAIL);
        }
        auto response = std::make_unique<Serialize::Response>();
        response->set_allocated_reg_response(reg_response.release());

        return response;
    }

    static ptr_proto_response_t create_text_response(const std::string& login, int room_id, const std::string& text) {
        auto text_response = std::make_unique<Serialize::TextResponse>();
        text_response->set_login(login);
        text_response->set_room_id(room_id);
        text_response->set_text(text);

        auto response = std::make_unique<Serialize::Response>();
        response->set_allocated_text_response(text_response.release());

        return response;
    }

    static work_buf_res_t serialize_response(ptr_header_t&& header_ptr, ptr_proto_response_t&& response_ptr) {
        work_buf_res_t __buffer = std::make_unique<uint8_t[]>(BUF_RES_LEN);
        header_ptr->SerializeToArray(__buffer.get(), sizeof(Serialize::Header));
        response_ptr->SerializeToArray(__buffer.get() + sizeof(Serialize::Header), header_ptr->length());

        return __buffer;
    }

    static work_buf_res_t serialize_request(ptr_header_t&& header_ptr, ptr_proto_request_t&& request_ptr) {
        work_buf_req_t __buffer = std::make_unique<uint8_t[]>(BUF_REQ_LEN);
        header_ptr->SerializeToArray(__buffer.get(), sizeof(Serialize::Header));
        request_ptr->SerializeToArray(__buffer.get() + sizeof(Serialize::Header), header_ptr->length());

        return __buffer;
    }


};

#endif // MSGFACTORY_H
