#ifndef CONTROLRESPONSE_H
#define CONTROLRESPONSE_H

#include <protocol/response/iresponse.h>

class ControlResponse: public IResponse
{
public:
    virtual ~ControlResponse() {}

    virtual TypeCommand get_type() const override final { return type_response; }
protected:
    const TypeCommand type_response = TypeCommand::Unknown;
};

// ************************************************************************************
class InputResponse : public ControlResponse {
public:
    InputResponse() {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
    }

    virtual const void* get_data() const override { return __data; }
    virtual const void* get_optional() const  override { return __data + Block::Header; }
    virtual void* get_data() override { return __data; }
    virtual void* get_optional()  override { return __data + Block::Header; }

    virtual uint16_t get_protocol_version() const override { return *(uint16_t*)__data; }
    virtual uint16_t get_type_data() const override { return *(uint16_t*) (__data + Block::VersionProtocol) ; }
    virtual uint32_t get_loginid() const {
        return *(uint32_t *)(__data+Block::VersionProtocol + Block::Command);
    }
    virtual uint32_t get_length_response() const override { return LengthResponse;}
    virtual void set_loginid(int32_t id = 0) {
        std::memcpy(__data+Block::Header, &id, Block::LoginId);
    }
protected:
    const TypeCommand type_response = TypeCommand::Unknown;
    static constexpr auto LengthResponse = Block::VersionProtocol + Block::Command + Block::LoginId;
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
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
    }
    RegistrationResponse(int32_t id) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
        std::memcpy(__data+Block::Header, &id, Block::LoginId);
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
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
    }
    AutorisationResponse(int32_t id) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_response, Block::Command);
        std::memcpy(__data+Block::Header, &id, Block::LoginId);
    }

private:
    const TypeCommand type_response = TypeCommand::AuthorisationResponse;
};

using input_res_ptr = std::shared_ptr<InputResponse>;
using reg_res_ptr = std::shared_ptr<RegistrationResponse>;
using autor_res_ptr = std::shared_ptr<AutorisationResponse>;


#endif // CONTROLRESPONSE_H
