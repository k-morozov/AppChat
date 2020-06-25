#ifndef COMMAND_TABLE_H
#define COMMAND_TABLE_H

#include <cstdint>

enum class TypeCommand : uint16_t {
    Unknown                     = 0xFFFF,
    RegistrationRequest         = 0x0001,
    RegistrationResponse        = 0x0002,
    AuthorizationRequest        = 0x0003,
    AuthorizationResponse       = 0x0004,
    EchoRequest                 = 0x0005,
    EchoResponse                = 0x0006,
    JoinRoomRequest             = 0x0007,
    JoinRoomResponse            = 0x0008,
    LeaveRoomRequest            = 0x000A,
};

#endif // COMMAND_TABLE_H
