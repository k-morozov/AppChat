#ifndef CONTROLRESPONSE_H
#define CONTROLRESPONSE_H

#include <protocol/response/iresponse.h>

class Response: public IResponse
{
public:
    virtual ~Response() {}

    Response() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
    }

    virtual const void* get_header() const override { return header; }
    virtual void* get_header() override { return header; }
    virtual const void* get_data() const override { return nullptr; }
    virtual void* get_data() override { return nullptr; }

    virtual uint16_t get_protocol_version() const override { return *(uint16_t*)header; }

    virtual TypeCommand get_type() const override { return TypeCommand::Unknown; }
    virtual TypeCommand get_type_data() const override { return (TypeCommand) *(header + Block::VersionProtocol); }

    virtual uint32_t get_length_response() const override { return 0; }

protected:
    char header[Block::Header];
};


// ************************************************************************************
class InputResponse : public Response {
public:
    InputResponse() {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
    }

    virtual const void* get_data() const  override { return __data; }
    virtual void* get_data()  override { return __data; }

    virtual int32_t get_loginid() const {
        return *(int32_t *)(__data);
    }
    virtual uint32_t get_length_response() const override { return LengthResponse;}
    virtual void set_loginid(int32_t id = 0) {
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
    RegistrationResponse(int32_t id) {
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
    AutorisationResponse(int32_t id) {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(header+Block::VersionProtocol, &type_response, Block::Command);
        std::memcpy(__data, &id, Block::LoginId);
    }

private:
    const TypeCommand type_response = TypeCommand::AuthorisationResponse;
};

using response_ptr = std::shared_ptr<Response>;
using input_res_ptr = std::shared_ptr<InputResponse>;
using reg_res_ptr = std::shared_ptr<RegistrationResponse>;
using autor_res_ptr = std::shared_ptr<AutorisationResponse>;


#endif // CONTROLRESPONSE_H
