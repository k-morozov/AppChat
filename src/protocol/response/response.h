#ifndef IRESPONSE_H
#define IRESPONSE_H

#include <protocol/command_table.h>

class Response: public IPacket
{
public:
    virtual ~Response() {}

    Response() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
    }

    virtual const void* get_header() const override { return header; }
    virtual void* get_header() override { return header; }
    virtual const void* get_data() const override { return nullptr; }
    virtual void* get_data() override { return nullptr; }

    virtual uint16_t get_protocol_version() const override { return *(uint16_t*)header; }

    virtual TypeCommand get_type() const override { return TypeCommand::Unknown; }
    virtual TypeCommand get_type_data() const override { return (TypeCommand) *(header + Block::VersionProtocol); }

    virtual uint32_t get_length_data() const override { return 0; }

protected:
    char header[Block::Header];
};

using response_ptr = std::shared_ptr<Response>;

#endif // IRESPONSE_H
