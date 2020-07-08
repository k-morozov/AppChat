#ifndef COMMAND_TABLE_H
#define COMMAND_TABLE_H

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <cassert>

constexpr uint32_t PROTOCOL_VERS = 2;

using identifier_t = int32_t;

namespace Block {
    constexpr uint8_t VersionProtocol   = 2;
    constexpr uint8_t Command           = 2;
    constexpr uint8_t LoginName         = 16;
    constexpr uint8_t Password          = 32;
    constexpr uint8_t LoginId           = sizeof(identifier_t);
    constexpr uint8_t RoomId            = sizeof(identifier_t);
    constexpr uint8_t TextMessage       = 255;

    constexpr uint8_t Header            = VersionProtocol + Command;
    constexpr uint8_t InputOption       = LoginName + Password;
}

enum class TypeCommand : uint16_t {
    Unknown                     = 0xFFFF,
    RegistrationRequest         = 0x0001,
    RegistrationResponse        = 0x0002,
    AuthorisationRequest        = 0x0003,
    AutorisationResponse        = 0x0004,
    EchoRequest                 = 0x0005,
    EchoResponse                = 0x0006,
    JoinRoomRequest             = 0x0007,
    JoinRoomResponse            = 0x0008,
    LeaveRoomRequest            = 0x0009,
};

static const std::unordered_map< TypeCommand, std::string> type_command_str = {
    {TypeCommand::RegistrationRequest, "RegistrationRequest"},
    {TypeCommand::RegistrationResponse, "RegistrationResponse"},
    {TypeCommand::AuthorisationRequest, "AuthorisationRequest"},
    {TypeCommand::AutorisationResponse, "AutorisationResponse"},
    {TypeCommand::EchoRequest, "EchoRequest"},
    {TypeCommand::EchoResponse, "EchoResponse"},
    {TypeCommand::JoinRoomRequest, "JoinRoomRequest"},

    {TypeCommand::Unknown, "Unknown"},
};
inline const std::string& get_command_str(TypeCommand command) {
    if (auto it=type_command_str.find(command); it!=type_command_str.end()) {
        return it->second;
    }
    assert(type_command_str.at(TypeCommand::Unknown)=="Unknown");
    return type_command_str.at(TypeCommand::Unknown);
}

enum class InputCode : int {
    RegistrOK = 1,
    AutorOK,
    BusyRegistr,
    IncorrectAutor
};

class IPacket {
public:
    virtual ~IPacket() {}

    virtual const void* get_header() const = 0;
    virtual void* get_header() = 0;
    virtual const void* get_data() const = 0;
    virtual void* get_data() = 0;

    virtual uint16_t get_protocol_version() const = 0;

    virtual TypeCommand get_type() const = 0;
    virtual TypeCommand get_type_data() const = 0;

    virtual uint32_t get_length_data() const = 0;
};

using packet_ptr = std::shared_ptr<IPacket>;

#endif // COMMAND_TABLE_H
