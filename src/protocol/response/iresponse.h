#ifndef IRESPONSE_H
#define IRESPONSE_H

#include <memory>
#include <protocol/command_table.h>


class IResponse {
public:
    virtual ~IResponse() {}

    virtual const void* get_header() const = 0;
    virtual void* get_header() = 0;
    virtual const void* get_data() const = 0;
    virtual void* get_data() = 0;

    virtual uint16_t get_protocol_version() const = 0;

    virtual TypeCommand get_type() const = 0;
    virtual TypeCommand get_type_data() const = 0;

    virtual uint32_t get_length_response() const = 0;
};

using iresponse_ptr = std::shared_ptr<IResponse>;
#endif // IRESPONSE_H
