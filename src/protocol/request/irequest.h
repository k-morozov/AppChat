#ifndef IREQUEST_H
#define IREQUEST_H

#include <array>
#include <memory>
#include <protocol/command_table.h>


class IRequest {
public:
    virtual ~IRequest() {}

    virtual TypeCommand get_type() const = 0;
    virtual uint16_t get_protocol_version() const = 0;
    virtual uint16_t get_type_command() const = 0;
    virtual uint32_t get_length_request() const = 0;
};





#endif // IREQUEST_H
