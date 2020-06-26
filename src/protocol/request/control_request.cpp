#include <protocol/request/control_request.h>

std::ostream& operator<<(std::ostream& os, const RegistrationRequest& request) {
    os << "protocol version=" << request.get_protocol_version()
        << ", type command=" << request.get_type_data()
        << ", login=" << request.get_login()
        << ", password=" << request.get_password();
    return os;
}

std::ostream& operator<<(std::ostream& os, const AutorisationRequest& request) {
    os << "protocol version=" << request.get_protocol_version()
        << ", type command=" << request.get_type_data()
        << ", login=" << request.get_login()
        << ", password=" << request.get_password();
    return os;
}

std::ostream& operator<<(std::ostream& os, const registr_req_ptr& request) {
    os << "protocol version=" << request->get_protocol_version()
        << ", type command=" << request->get_type_data()
        << ", login=" << request->get_login()
        << ", password=" << request->get_password();
    return os;
}
std::ostream& operator<<(std::ostream& os, const autor_req_ptr& request) {
    os << "protocol version=" << request->get_protocol_version()
        << ", type command=" << request->get_type_data()
        << ", login=" << request->get_login()
        << ", password=" << request->get_password();
    return os;
}
