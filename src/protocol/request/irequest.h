#ifndef IREQUEST_H
#define IREQUEST_H

#include <memory>
#include <protocol/command_table.h>


class IRequest {
public:
    virtual ~IRequest() {}

    virtual std::string parse() const = 0;
    virtual void write_block(std::size_t pos, void* data, std::size_t count_bytes) = 0;
    virtual void* get_data() = 0;
    virtual void* read_block(std::size_t pos) = 0;
    virtual TypeCommand get_type() const = 0;


private:
//    void *__data;
//    std::size_t length_data;
//    TypeCommand __type;

};





#endif // IREQUEST_H
