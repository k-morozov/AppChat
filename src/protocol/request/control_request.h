#ifndef CONTROLREQUEST_H
#define CONTROLREQUEST_H

#include <protocol/request/irequest.h>

class ControlRequest: public IRequest
{
public:
    //ControlRequest();
    virtual ~ControlRequest() {}

    virtual TypeCommand get_type() const override final { return type_request; }

protected:
    const TypeCommand type_request = TypeCommand::Unknown;
};

// ************************************************************************************
class InputRequest : public ControlRequest {
public:
    virtual uint16_t get_protocol_version() const override { return *(uint16_t*)__data; }
    virtual uint16_t get_type_command() const override { return *(uint16_t*) (__data + Block::VersionProtocol) ; }
    virtual const char* get_login() const {
        return __data+Block::VersionProtocol + Block::Command;
    }
    virtual const char* get_password() const {
        return __data+Block::VersionProtocol + Block::Command + Block::LoginName;
    }

    virtual uint32_t get_length_request() const override { return LengthRequest;}

protected:
    const TypeCommand type_request = TypeCommand::Unknown;
    static constexpr auto LengthRequest = Block::VersionProtocol + Block::Command + Block::LoginName + Block::Password;
    char __data[LengthRequest];
};

// *************************************************************************************************


// *** |protocol vers.|    ->|RegistrationRequest|  ->|login 8 bytes|         ->|password 32 bytes|
class RegistrationRequest : public InputRequest {
public:
    RegistrationRequest(const char* login, const char* password) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);

        std::snprintf(__data+Block::VersionProtocol+Block::Command,
                 Block::LoginName, "%s", login);
        std::snprintf(__data+Block::VersionProtocol+Block::Command + Block::LoginName,
                 Block::Password, "%s", password);
    }


private:
    const TypeCommand type_request = TypeCommand::RegistrationRequest;
};


#endif // CONTROLREQUEST_H
