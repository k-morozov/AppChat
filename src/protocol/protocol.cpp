#include <protocol/protocol.h>

void Message::encode_header() {
    char header[header_size+1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length));
    std::memcpy(data, header, header_size);
}

bool Message::decode_header() {
    char header[header_size+1] = "";
    std::strncat(header, data, header_size);
    body_length = atoi(header);
    if (body_length>max_body_size) {
        body_length = 0;
        return false;
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Message& mes) {
    os << "message:"
       <<  " login=<" <<mes.get_buf_str_login() << ">"
       << ", login_id=<" << mes.get_login_id() << ">"
       << ", room_id=<" << mes.get_room_id() << ">"
       << ", message=<" << mes.get_buf_body() << ">";
    return os;
}
