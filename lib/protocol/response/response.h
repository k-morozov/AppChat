#ifndef IRESPONSE_H
#define IRESPONSE_H

#include <command_table.h>

class Response: public IPacket
{
public:
    virtual ~Response() {}

    Response() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);

        DateTime datetime(boost::posix_time::second_clock::universal_time());

        std::memcpy(header + Block::VersionProtocol + Block::Command, &datetime, Block::Datetime);
    }

    virtual const void* get_header() const override { return header; }
    virtual void* get_header() override { return header; }
    virtual const void* get_data() const override { return nullptr; }
    virtual void* get_data() override { return nullptr; }

    virtual uint16_t get_protocol_version() const override { return *(uint16_t*)header; }

    virtual TypeCommand get_type() const override { return TypeCommand::Unknown; }
    virtual TypeCommand get_type_data() const override { return (TypeCommand) *(header + Block::VersionProtocol); }

    virtual DateTime get_datetime() const override
    {
        DateTime dt;
        std::memcpy(&dt, header + Block::VersionProtocol + Block::Command, Block::Datetime);
        return dt;
    }

    virtual uint32_t get_length_data() const override { return 0; }

protected:
    char header[Block::Header];
};

using response_ptr = std::shared_ptr<Response>;

#endif // IRESPONSE_H
