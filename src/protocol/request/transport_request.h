#ifndef TRANSPORTREQUEST_H
#define TRANSPORTREQUEST_H

#include <protocol/request/irequest.h>

class TransportRequest: public IRequest {
public:
    virtual ~TransportRequest() {}

    virtual std::string parse() const = 0;
    virtual void write_block(std::size_t pos, void* data, std::size_t count_bytes) = 0;
    virtual void* get_data() = 0;
    virtual void* read_block(std::size_t pos) = 0;
    virtual TypeCommand get_type() const = 0;
};

#endif // TRANSPORTREQUEST_H
