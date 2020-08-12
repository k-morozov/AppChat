#ifndef TRANSPORT_RESPONSE_H
#define TRANSPORT_RESPONSE_H

#include "response.h"

class TransportResponse : public Response {
public:
    TransportResponse() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
    }

    virtual const void* get_data() const  override { return __data; }
    virtual void* get_data()  override { return __data; }

    virtual const char* get_login() const {
        return __data;
    }

    virtual identifier_t get_roomid() const {
        /**
          * @note replace -> *(identifier_t *)(__data+offset);
          * Misaligned Integer Pointer Assignment in C
          * The compiler can often safely optimize calls to memcpy,
          * even if the arguments are unaligned.
          * */
        identifier_t roomid;
        std::memcpy(&roomid, __data + Block::LoginName, sizeof(identifier_t));
        return roomid;
    }

    virtual uint32_t get_length_data() const override { return LengthResponse;}

protected:
    static constexpr auto LengthResponse = Block::LoginName + Block::RoomId + Block::TextMessage;
    char __data[LengthResponse];
};

class TextResponse : public TransportResponse {
public:
    TextResponse(identifier_t roomid = 0) {
        //std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);

        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
    }

    TextResponse(const std::string& login, const std::string& text, identifier_t roomid = 0) {
        //std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
        std::snprintf(__data+Block::LoginName + Block::RoomId, Block::TextMessage, "%s", text.data());
    }

    TextResponse(const std::string& login, DateTime dt, const std::string& text, identifier_t roomid = 0) {
        //std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
        std::memcpy(header+Block::VersionProtocol+Block::Command, &dt, Block::Datetime);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
        std::snprintf(__data+Block::LoginName + Block::RoomId, Block::TextMessage, "%s", text.data());
    }

    TextResponse(response_ptr response) {
        std::memcpy(header, response->get_header(), Block::Header);

        std::memset(__data+Block::LoginName, 0, Block::RoomId);
    }

    const char* get_message() const {
        return __data+Block::LoginName+Block::RoomId;
    }
private:
    const TypeCommand type_Response = TypeCommand::EchoResponse;
};

using transport_response_ptr = std::shared_ptr<TransportResponse>;
using text_response_ptr = std::shared_ptr<TextResponse>;

#endif // TRANSPORT_RESPONSE_H
