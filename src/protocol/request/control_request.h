#ifndef CONTROL_REQUEST_H
#define CONTROL_REQUEST_H

#include <protocol/request/request.h>

class ControlRequest : public Request {
public:
    ControlRequest() {
        std::memcpy(header, &PROTOCOL_VERS, Block::VersionProtocol);
    }
    virtual const void* get_data() const  override { return __data; }
    virtual void* get_data()  override { return __data; }

    virtual identifier_t get_roomid() const {
        return *(identifier_t *)(__data);
    }

    virtual uint32_t get_length_data() const override { return LengthRequest;}

protected:
    static constexpr auto LengthRequest = Block::RoomId;
    char __data[LengthRequest];
};

class JoinRoomRequest : public   ControlRequest {
public:
    JoinRoomRequest(identifier_t roomid=0) {
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::VersionProtocol);
        std::memcpy(__data, &roomid, Block::RoomId);
    }

    JoinRoomRequest(request_ptr request) {
        std::memcpy(header, request->get_header(), Block::Header);
        std::memcpy(header+Block::VersionProtocol, &type_request, Block::VersionProtocol);
    }
private:
    const TypeCommand type_request = TypeCommand::JoinRoomRequest;
};

using join_room_request_ptr = std::shared_ptr<JoinRoomRequest>;
#endif // CONTROL_REQUEST_H
