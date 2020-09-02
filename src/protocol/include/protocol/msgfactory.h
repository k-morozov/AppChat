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

constexpr std::size_t SIZE_HEADER = 3*(sizeof(int32_t)+1);

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
        header->set_length(length);

        header->set_version(1);
        header->set_command(static_cast<google::protobuf::int32>(command));
//        header->set_time(0);

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

    static ptr_proto_response_t create_join_room_response(int new_room_id, bool flag) {
        auto jr_response = std::make_unique<Serialize::JoinRoomResponse>();
        jr_response->set_room_id(new_room_id);
        if (flag) {
            jr_response->set_status(Serialize::STATUS::OK);
        }
        else {
            jr_response->set_status(Serialize::STATUS::FAIL);
        }

        auto response = std::make_unique<Serialize::Response>();
        response->set_allocated_join_room_response(jr_response.release());

        return response;
    }

    static std::vector<uint8_t> serialize_response(ptr_header_t&& header_ptr, ptr_proto_response_t&& response_ptr) {
        std::vector<uint8_t> bin_buffer(SIZE_HEADER + response_ptr->ByteSizeLong());
//        header_ptr->set_length(request_ptr->ByteSizeLong());
        header_ptr->SerializeToArray(bin_buffer.data(), SIZE_HEADER);
        response_ptr->SerializeToArray(bin_buffer.data() + SIZE_HEADER, response_ptr->ByteSizeLong());

        return bin_buffer;
    }

    static std::vector<uint8_t> serialize_request(ptr_header_t&& header_ptr, ptr_proto_request_t&& request_ptr) {
        std::vector<uint8_t> bin_buffer(SIZE_HEADER + request_ptr->ByteSizeLong());
//        header_ptr->set_length(request_ptr->ByteSizeLong());
        header_ptr->SerializeToArray(bin_buffer.data(), SIZE_HEADER);
        request_ptr->SerializeToArray(bin_buffer.data() + SIZE_HEADER, request_ptr->ByteSizeLong());

        return bin_buffer;
    }


};

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
