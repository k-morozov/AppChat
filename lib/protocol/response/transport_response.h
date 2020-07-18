#ifndef TRANSPORT_RESPONSE_H
#define TRANSPORT_RESPONSE_H

#include <response/response.h>

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
        return *(identifier_t *)(__data+Block::LoginName);
    }

    virtual uint32_t get_length_data() const override { return LengthResponse;}

protected:
    static constexpr auto LengthResponse = Block::LoginName + Block::RoomId + Block::DateTime + Block::TextMessage;
    char __data[LengthResponse];
};

class TextResponse : public TransportResponse {
public:
    TextResponse(identifier_t roomid = 0) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);

        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
    }

    TextResponse(const std::string& login, const std::string& text, identifier_t roomid = 0) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
        std::snprintf(__data+Block::LoginName + Block::RoomId + Block::DateTime, Block::TextMessage, "%s", text.data());
    }

    TextResponse(const std::string& login, const std::string& datetime, const std::string& text, identifier_t roomid = 0) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
        std::snprintf(__data+Block::LoginName + Block::RoomId, Block::DateTime, "%s", datetime.data());
        std::snprintf(__data+Block::LoginName + Block::RoomId + Block::DateTime, Block::TextMessage, "%s", text.data());
    }

    TextResponse(response_ptr response) {
        std::memcpy(header, response->get_header(), Block::Header);

        std::memset(__data+Block::LoginName, 0, Block::RoomId + Block::DateTime);
    }

    const char* get_datetime() const {
        return __data+Block::LoginName+Block::RoomId;
    }

    const char* get_message() const {
        return __data+Block::LoginName+Block::RoomId+Block::DateTime;
    }
private:
    const TypeCommand type_Response = TypeCommand::EchoResponse;
};

using transport_response_ptr = std::shared_ptr<TransportResponse>;
using text_response_ptr = std::shared_ptr<TextResponse>;

#endif // TRANSPORT_RESPONSE_H
