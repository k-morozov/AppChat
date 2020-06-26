#ifndef COMMAND_TABLE_H
#define COMMAND_TABLE_H

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>

constexpr uint32_t PROTOCOL_VERS = 23;

namespace Block {
    constexpr uint8_t VersionProtocol   = 2;
    constexpr uint8_t Command           = 2;
    constexpr uint8_t LoginName         = 8;
    constexpr uint8_t Password          = 32;
    constexpr uint8_t LoginId           = 4;

    constexpr uint8_t Header            = VersionProtocol + Command;
    constexpr uint8_t InputOption       = LoginName + Password;
}


enum class TypeCommand : uint16_t {
    Unknown                     = 0xFFFF,
    RegistrationRequest         = 0x0001,
    RegistrationResponse        = 0x0002,
    AuthorisationRequest        = 0x0003,
    AuthorisationResponse       = 0x0004,
    EchoRequest                 = 0x0005,
    EchoResponse                = 0x0006,
    JoinRoomRequest             = 0x0007,
    JoinRoomResponse            = 0x0008,
    LeaveRoomRequest            = 0x000A,
};


#endif // COMMAND_TABLE_H
