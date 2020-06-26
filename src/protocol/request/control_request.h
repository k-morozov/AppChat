#ifndef CONTROLREQUEST_H
#define CONTROLREQUEST_H

#include <protocol/request/irequest.h>

class ControlRequest: public IRequest
{
public:
    //ControlRequest();
    virtual ~ControlRequest() {}

    virtual void init(const void*) override = 0;
    virtual TypeCommand get_type() const override final { return type_request; }
protected:
    const TypeCommand type_request = TypeCommand::Unknown;
};

// ************************************************************************************
class InputRequest : public ControlRequest {
public:
    InputRequest() {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);
    }
    virtual void init(const void* from) override {
        std::memcpy(__data, from, LengthRequest);
    }
    virtual const void* get_data() const override { return __data; }
    virtual void* get_data() override { return __data; }
    virtual uint16_t get_protocol_version() const override { return *(uint16_t*)__data; }
    virtual uint16_t get_type_data() const override { return *(uint16_t*) (__data + Block::VersionProtocol) ; }
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
/**
 * @brief The RegistrationRequest class
 * @todo add more settings for registration: email, name ...
 */
class RegistrationRequest : public InputRequest {
public:
    RegistrationRequest() {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);
    }
    RegistrationRequest(const char* login, const char* password) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);

        std::snprintf(__data+Block::VersionProtocol+Block::Command,
                 Block::LoginName, "%s", login);
        std::snprintf(__data+Block::VersionProtocol+Block::Command + Block::LoginName,
                 Block::Password, "%s", password);
    }
    RegistrationRequest(const std::string& login, const std::string& password) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);

        std::snprintf(__data+Block::VersionProtocol+Block::Command,
                 Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::VersionProtocol+Block::Command + Block::LoginName,
                 Block::Password, "%s", password.data());
    }

private:
    const TypeCommand type_request = TypeCommand::RegistrationRequest;
};

/**
 * @brief The AutorisationRequest class
 */
class AutorisationRequest : public InputRequest {
public:
    AutorisationRequest() {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);
    }
    AutorisationRequest(const char* login, const char* password) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);

        std::snprintf(__data+Block::VersionProtocol+Block::Command,
                 Block::LoginName, "%s", login);
        std::snprintf(__data+Block::VersionProtocol+Block::Command + Block::LoginName,
                 Block::Password, "%s", password);
    }
    AutorisationRequest(const std::string& login, const std::string& password) {
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);

        std::snprintf(__data+Block::VersionProtocol+Block::Command,
                 Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::VersionProtocol+Block::Command + Block::LoginName,
                 Block::Password, "%s", password.data());
    }


private:
    const TypeCommand type_request = TypeCommand::AuthorisationRequest;
};



// *************************************************************************************************
using input_req_ptr = std::shared_ptr<InputRequest>;
using registr_req_ptr = std::shared_ptr<RegistrationRequest>;
using autor_req_ptr = std::shared_ptr<AutorisationRequest>;

std::ostream& operator<<(std::ostream& os, const RegistrationRequest& request);
std::ostream& operator<<(std::ostream& os, const AutorisationRequest& request);
std::ostream& operator<<(std::ostream& os, const registr_req_ptr& request);
std::ostream& operator<<(std::ostream& os, const autor_req_ptr& request);
std::ostream& operator<<(std::ostream& os, const InputRequest& request);
#endif // CONTROLREQUEST_H
