#ifndef CONTROLRESPONSE_H
#define CONTROLRESPONSE_H

#include <protocol/response/response.h>

class InputResponse : public Response {
public:
    InputResponse() {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
    }

    virtual const void* get_data() const  override { return __data; }
    virtual void* get_data()  override { return __data; }

    virtual identifier_t get_loginid() const {
        return *(identifier_t *)(__data);
    }
    virtual uint32_t get_length_data() const override { return LengthResponse;}
    virtual void set_loginid(identifier_t id = 0) {
        std::memcpy(__data, &id, Block::LoginId);
    }
protected:
    const TypeCommand type_response = TypeCommand::Unknown;
    static constexpr auto LengthResponse = Block::LoginId;
    char __data[LengthResponse];
};

// *************************************************************************************************
/**
 * @brief The RegistrationResponse class
 * @todo add more settings for registration: email, name ...
 */
class RegistrationResponse : public InputResponse {
public:
    RegistrationResponse() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_response, Block::Command);
    }
    RegistrationResponse(identifier_t id) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_response, Block::Command);
        std::memcpy(__data, &id, Block::LoginId);
    }

private:
    const TypeCommand type_response = TypeCommand::RegistrationResponse;
};

/**
 * @brief The AutorisationResponse class
 */
class AutorisationResponse : public InputResponse {
public:
    AutorisationResponse() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_response, Block::Command);
    }
    AutorisationResponse(identifier_t id) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_response, Block::Command);
        std::memcpy(__data, &id, Block::LoginId);
    }
    AutorisationResponse(const Response& packet) {
        std::memcpy(header, packet.get_data(), Block::Header);
    }
    AutorisationResponse(response_ptr packet) {
        std::memcpy(header, packet->get_data(), Block::Header);
    }
private:
    const TypeCommand type_response = TypeCommand::AutorisationResponse;
};

using response_ptr = std::shared_ptr<Response>;
using input_res_ptr = std::shared_ptr<InputResponse>;
using reg_res_ptr = std::shared_ptr<RegistrationResponse>;
using autor_response_ptr = std::shared_ptr<AutorisationResponse>;


#endif // CONTROLRESPONSE_H
