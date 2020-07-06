#ifndef CONTROLREQUEST_H
#define CONTROLREQUEST_H

#include <protocol/request/request.h>

class InputRequest : public Request {
public:
    InputRequest():Request() {
//        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
    }
    virtual const void* get_data() const  override { return __data; }
    virtual void* get_data()  override { return __data; }

    virtual const char* get_login() const {
        return __data;
    }
    virtual const char* get_password() const {
        return __data+Block::LoginName;
    }

    virtual uint32_t get_length_data() const override { return LengthRequest;}

protected:
    static constexpr auto LengthRequest = Block::LoginName + Block::Password;
    char __data[LengthRequest];
};

// *************************************************************************************************
/**
 * @brief The RegistrationRequest class
 * @todo add more settings for registration: email, name ...
 */
class RegistrationRequest : public InputRequest {
public:
    RegistrationRequest():InputRequest() {
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
    }

    RegistrationRequest(const std::string& login, const std::string& password):InputRequest() {
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::LoginName, Block::Password, "%s", password.data());
    }

    RegistrationRequest(request_ptr req_ptr, const std::string& login, const std::string& password) {
        std::memcpy(header, req_ptr->get_header(), Block::Header);

        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::LoginName, Block::Password, "%s", password.data());
    }
    RegistrationRequest(request_ptr req_ptr) {
        std::memcpy(header, req_ptr->get_header(), Block::Header);
    }
private:
    const TypeCommand type_request = TypeCommand::RegistrationRequest;
};


class AutorisationRequest : public InputRequest {
public:
    AutorisationRequest() {
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
    }
    AutorisationRequest(const char* login, const char* password) {
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login);
        std::snprintf(__data+Block::LoginName, Block::Password, "%s", password);
    }
    AutorisationRequest(const std::string& login, const std::string& password) {
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::Command);
        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::LoginName, Block::Password, "%s", password.data());
    }

    AutorisationRequest(request_ptr req_ptr, const std::string& login, const std::string& password) {
        std::memcpy(header, req_ptr->get_header(), Block::Header);

        std::snprintf(__data, Block::LoginName, "%s", login.data());
        std::snprintf(__data+Block::LoginName, Block::Password, "%s", password.data());
    }
    AutorisationRequest(request_ptr req_ptr) {
        std::memcpy(header, req_ptr->get_header(), Block::Header);
    }
private:
    const TypeCommand type_request = TypeCommand::AuthorisationRequest;
};



// *************************************************************************************************

using input_request_ptr = std::shared_ptr<InputRequest>;
using registr_request_ptr = std::shared_ptr<RegistrationRequest>;
using autor_request_ptr = std::shared_ptr<AutorisationRequest>;

std::ostream& operator<<(std::ostream& os, const RegistrationRequest& request);
std::ostream& operator<<(std::ostream& os, const AutorisationRequest& request);

std::ostream& operator<<(std::ostream& os, const InputRequest& request);
#endif // CONTROLREQUEST_H
