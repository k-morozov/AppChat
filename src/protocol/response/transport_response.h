#ifndef TRANSPORT_RESPONSE_H
#define TRANSPORT_RESPONSE_H

#include <protocol/response/response.h>

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

    virtual int32_t get_roomid() const {
        return 0;
    }

    virtual uint32_t get_length_data() const override { return LengthResponse;}

protected:
    static constexpr auto LengthResponse = Block::LoginName + Block::TextMessage;
    char __data[LengthResponse];
};

class TextResponse : public TransportResponse {
public:
    TextResponse() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
    }

    TextResponse(const std::string& login, const std::string& text) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::LoginName, Block::TextMessage, "%s", text.data());
    }

    TextResponse(response_ptr response) {
        std::memcpy(header, response->get_header(), Block::Header);
//        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
//        std::memcpy(header+Block::VersionProtocol, &type_Response, Block::Command);
    }

    const char* get_message() const {
        return __data+Block::LoginName;
    }
private:
    const TypeCommand type_Response = TypeCommand::EchoResponse;
};

using transport_response_ptr = std::shared_ptr<TransportResponse>;
using text_response_ptr = std::shared_ptr<TextResponse>;

#endif // TRANSPORT_RESPONSE_H
