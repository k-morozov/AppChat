#ifndef TRANSPORTREQUEST_H
#define TRANSPORTREQUEST_H

#include "request.h"

class TransportRequest : public Request {
public:
    TransportRequest() {
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

    virtual uint32_t get_length_data() const override { return LengthRequest;}

protected:
    static constexpr auto LengthRequest = Block::LoginName + Block::RoomId + Block::TextMessage;
    char __data[LengthRequest];
};

class TextRequest : public TransportRequest {
public:
    TextRequest() {
//        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
    }

    TextRequest(const std::string& login, identifier_t roomid, const std::string& text) {
//        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::memcpy(__data+Block::LoginName, &roomid, Block::RoomId);
        std::snprintf(__data+Block::LoginName+Block::RoomId, Block::TextMessage, "%s", text.data());
    }

    TextRequest(request_ptr request) {
        std::memcpy(header, request->get_header(), Block::Header);
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
    }

    const char* get_message() const {
        return __data+Block::LoginName+Block::RoomId;
    }
private:
    const TypeCommand type_request = TypeCommand::EchoRequest;
};

using text_request_ptr = std::shared_ptr<TextRequest>;

#endif // TRANSPORTREQUEST_H
