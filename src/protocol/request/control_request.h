#ifndef CONTROLREQUEST_H
#define CONTROLREQUEST_H

#include <protocol/request/irequest.h>

class ControlRequest: public IRequest
{
public:
    ControlRequest();
    virtual ~ControlRequest() {}

    virtual void write_block(std::size_t pos, const void* data, std::size_t count_bytes) = 0;
    virtual void* get_data() = 0;
    virtual void* read_block(std::size_t pos) = 0;
    virtual TypeCommand get_type() const = 0;

};

// ************************************************************************************
class InputRequest : public ControlRequest {
public:
    virtual ~InputRequest() {}

    virtual void write_block(std::size_t pos, const void* from, std::size_t count_bytes) override {
        std::memcpy(__data+pos, from, count_bytes);
    }

    virtual void* get_data() override {
        return __data;
    }
    virtual void* read_block(std::size_t pos) override {
        return __data+pos;
    }
    virtual TypeCommand get_type() const override { return type_request; }

protected:
    const TypeCommand type_request = TypeCommand::Unknown;
    static constexpr auto LengthRequest = Block::VersionProtocol + Block::Command + Block::LoginName + Block::Password;
    char __data[LengthRequest];
};

class ChanelControlRequest : public ControlRequest {

};


// *************************************************************************************************
// *** |protocol vers.|    ->|RegistrationRequest|  ->|login 8 bytes|         ->|password 32 bytes|
class RegistrationRequest : public InputRequest {
public:
    RegistrationRequest(const char* login, const char* password) {
        std::cout << std::string(login) << " and " << std::string(password) << std::endl;
        std::memcpy(__data, &PROTOCOL_VERS, Block::VersionProtocol);
        std::memcpy(__data+Block::VersionProtocol, &type_request, Block::Command);

//        std::memcpy(__data+Block::VersionProtocol+Block::Command, login, Block::LoginName);
//        std::memcpy(__data+Block::VersionProtocol+Block::Command+Block::LoginName, password, Block::Password);
    }

    uint16_t get_protocol_version() const { return *(uint16_t*)__data; }
    uint16_t get_type_command() const { return *(uint16_t*) (__data + +Block::VersionProtocol) ; }
private:
    const TypeCommand type_request = TypeCommand::RegistrationRequest;
};


#endif // CONTROLREQUEST_H
