#ifndef IREQUEST_H
#define IREQUEST_H

#include <array>
#include <memory>
#include <protocol/command_table.h>


class IRequest {
public:
    virtual ~IRequest() {}

    virtual void write_block(std::size_t pos, const void* data, std::size_t count_bytes) = 0;
    virtual void* get_data() = 0;
    virtual void* read_block(std::size_t pos) = 0;
    virtual TypeCommand get_type() const = 0;
};





#endif // IREQUEST_H
