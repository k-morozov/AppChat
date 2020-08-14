#include <boost/test/unit_test.hpp>

#include "protocol/protocol.h"
#include <memory>

BOOST_AUTO_TEST_CASE(response_registration_basic) {
    identifier_t id = 12345;
    RegistrationResponse  response(id);

    BOOST_CHECK(response.get_protocol_version() == PROTOCOL_VERS);
    BOOST_CHECK(response.get_type_data() == TypeCommand::RegistrationResponse);
    BOOST_CHECK(response.get_loginid() == id);
}

BOOST_AUTO_TEST_CASE(response_autorisation_basic) {
    identifier_t id = 1234567890;
    AutorisationResponse  response(id);

    BOOST_CHECK(response.get_protocol_version() == PROTOCOL_VERS);
    BOOST_CHECK(response.get_type_data() == TypeCommand::AutorisationResponse);
    BOOST_CHECK(response.get_loginid() == id);
}

BOOST_AUTO_TEST_CASE(response_registration_basic_change_id) {
    identifier_t id = 12345;
    RegistrationResponse  response(id);
    id = 123;
    response.set_loginid(id);
    BOOST_CHECK(response.get_protocol_version() == PROTOCOL_VERS);
    BOOST_CHECK(response.get_type_data() == TypeCommand::RegistrationResponse);
    BOOST_CHECK(response.get_loginid() == id);

    response.set_loginid();
    BOOST_CHECK(response.get_protocol_version() == PROTOCOL_VERS);
    BOOST_CHECK(response.get_type_data() == TypeCommand::RegistrationResponse);
    BOOST_CHECK(response.get_loginid() == 0);
}

BOOST_AUTO_TEST_CASE(response_autorisation_basic_change_id) {
    identifier_t id = 1234567890;
    AutorisationResponse  response(id);
    id = 123;
    response.set_loginid(id);

    BOOST_CHECK(response.get_protocol_version() == PROTOCOL_VERS);
    BOOST_CHECK(response.get_type_data() == TypeCommand::AutorisationResponse);
    BOOST_CHECK(response.get_loginid() == id);

    response.set_loginid();
    BOOST_CHECK(response.get_protocol_version() == PROTOCOL_VERS);
    BOOST_CHECK(response.get_type_data() == TypeCommand::AutorisationResponse);
    BOOST_CHECK(response.get_loginid() == 0);
}
