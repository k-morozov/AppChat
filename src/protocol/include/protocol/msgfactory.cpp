#include "msgfactory.h"

namespace Protocol {
    MsgFactory::MsgFactory()
    {

    }


    ptr_proto_request_t MsgFactory::create_input_request(const std::string& login, const std::string& password) {
        auto in_request = std::make_unique<Serialize::InRequest>();
        in_request->set_login(login);
        in_request->set_password(password);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_input_request(in_request.release());

        return request;
    }

    ptr_proto_request_t MsgFactory::create_reg_request(const std::string& login, const std::string& password) {
        auto in_request = std::make_unique<Serialize::RegRequest>();
        in_request->set_login(login);
        in_request->set_password(password);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_register_request(in_request.release());

        return request;
    }

    ptr_proto_request_t MsgFactory::join_room_request(int room_id) {
        auto jr_request = std::make_unique<Serialize::JoinRoomRequest>();
        jr_request->set_room_id(room_id);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_join_room_request(jr_request.release());

        return request;
    }

    ptr_proto_request_t MsgFactory::create_text_request(const std::string& login, int room_id, const std::string& text) {
        auto text_request = std::make_unique<Serialize::TextRequest>();
        text_request->set_login(login);
        text_request->set_room_id(room_id);
        text_request->set_text(text);

        auto request = std::make_unique<Serialize::Request>();
        request->set_allocated_text_request(text_request.release());

        return request;
    }

    ptr_header_t MsgFactory::create_header(TypeCommand command, int32_t length) {
        auto header = std::make_unique<Serialize::Header>();
        header->set_length(length);

        header->set_version(1);
        header->set_command(static_cast<google::protobuf::int32>(command));
//        header->set_time(0);

        return header;
    }

    ptr_proto_response_t MsgFactory::create_input_response(int client_id) {
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

    ptr_proto_response_t MsgFactory::create_reg_response(int client_id) {
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

    ptr_proto_response_t MsgFactory::create_text_response(const std::string& login, int room_id, const std::string& text) {
        auto text_response = std::make_unique<Serialize::TextResponse>();
        text_response->set_login(login);
        text_response->set_room_id(room_id);
        text_response->set_text(text);

        auto response = std::make_unique<Serialize::Response>();
        response->set_allocated_text_response(text_response.release());

        return response;
    }

    ptr_proto_response_t MsgFactory::create_join_room_response(int new_room_id, bool flag) {
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

    std::vector<uint8_t> MsgFactory::serialize_response(ptr_header_t&& header_ptr, ptr_proto_response_t&& response_ptr) {
        std::vector<uint8_t> bin_buffer(SIZE_HEADER + response_ptr->ByteSizeLong());
//        header_ptr->set_length(request_ptr->ByteSizeLong());
        header_ptr->SerializeToArray(bin_buffer.data(), SIZE_HEADER);
        response_ptr->SerializeToArray(bin_buffer.data() + SIZE_HEADER, response_ptr->ByteSizeLong());

        return bin_buffer;
    }

    std::vector<uint8_t> MsgFactory::serialize_request(ptr_header_t&& header_ptr, ptr_proto_request_t&& request_ptr) {
        std::vector<uint8_t> bin_buffer(SIZE_HEADER + request_ptr->ByteSizeLong());
//        header_ptr->set_length(request_ptr->ByteSizeLong());
        header_ptr->SerializeToArray(bin_buffer.data(), SIZE_HEADER);
        request_ptr->SerializeToArray(bin_buffer.data() + SIZE_HEADER, request_ptr->ByteSizeLong());

        return bin_buffer;
    }


}
