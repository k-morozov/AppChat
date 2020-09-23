#include <boost/test/unit_test.hpp>

#include "protocol/protocol.h"
#include <memory>

BOOST_AUTO_TEST_CASE(request_reg_1) {
    std::string login = "vasiliy";
    std::string passsword = "a1s22g_1";

    auto reg_request_bin = Protocol::MsgFactory::create_reg_request(login, passsword);

    BOOST_CHECK(reg_request_bin->has_register_request());
    BOOST_CHECK(!reg_request_bin->has_input_request());
    BOOST_CHECK(!reg_request_bin->has_text_request());
    BOOST_CHECK(!reg_request_bin->has_join_room_request());
}

BOOST_AUTO_TEST_CASE(request_reg_2) {
    std::string login = "vasiliysdasdasdasadsdadsad asdsad";
    std::string passsword = "a1s22g_1sadasdsadsadasdsad1231231242142140856856";

    auto reg_request_bin = Protocol::MsgFactory::create_reg_request(login, passsword);

    BOOST_CHECK(reg_request_bin->has_register_request());
    BOOST_CHECK(!reg_request_bin->has_input_request());
    BOOST_CHECK(!reg_request_bin->has_text_request());
    BOOST_CHECK(!reg_request_bin->has_join_room_request());
}

BOOST_AUTO_TEST_CASE(request_reg_3) {
    std::string login = "";
    std::string passsword = "";

    auto reg_request_bin = Protocol::MsgFactory::create_reg_request(login, passsword);

    BOOST_CHECK(reg_request_bin->has_register_request());
    BOOST_CHECK(!reg_request_bin->has_input_request());
    BOOST_CHECK(!reg_request_bin->has_text_request());
    BOOST_CHECK(!reg_request_bin->has_join_room_request());
}

BOOST_AUTO_TEST_CASE(registration_request_1) {
    std::string login = "vasiliy";
    std::string passsword = "a1s22g_1";

    auto reg_request_bin = Protocol::MsgFactory::create_reg_request(login, passsword);

    BOOST_CHECK(reg_request_bin->register_request().login() == login);
    BOOST_CHECK(reg_request_bin->register_request().password() == passsword);
}

BOOST_AUTO_TEST_CASE(registration_request_2) {
    std::string login = "vasilsdasafmdskndlskn dsmfkkdsnfkndsnfsdlnfkldsnflknsdfkndslkfniy";
    std::string passsword = "a1s22g_133289408324877gds8f7gds7gsd7g987dsfg87sd89g78ds7g7sdg987sd8g7987dsg";

    auto reg_request_bin = Protocol::MsgFactory::create_reg_request(login, passsword);

    BOOST_CHECK(reg_request_bin->register_request().login() == login);
    BOOST_CHECK(reg_request_bin->register_request().password() == passsword);
}

//*********************************************************************************************************************
BOOST_AUTO_TEST_CASE(request_auto_1) {
    std::string login = "vasiliy";
    std::string passsword = "a1s22g_1";

    auto in_request_bin = Protocol::MsgFactory::create_input_request(login, passsword);

    BOOST_CHECK(!in_request_bin->has_register_request());
    BOOST_CHECK(in_request_bin->has_input_request());
    BOOST_CHECK(!in_request_bin->has_text_request());
    BOOST_CHECK(!in_request_bin->has_join_room_request());
}

BOOST_AUTO_TEST_CASE(request_auto_2) {
    std::string login = "vasiliysdasdasdasadsdadsad asdsad";
    std::string passsword = "a1s22g_1sadasdsadsadasdsad1231231242142140856856";

    auto in_request_bin = Protocol::MsgFactory::create_input_request(login, passsword);

    BOOST_CHECK(!in_request_bin->has_register_request());
    BOOST_CHECK(in_request_bin->has_input_request());
    BOOST_CHECK(!in_request_bin->has_text_request());
    BOOST_CHECK(!in_request_bin->has_join_room_request());
}

BOOST_AUTO_TEST_CASE(request_auto_3) {
    std::string login = "";
    std::string passsword = "";

    auto in_request_bin = Protocol::MsgFactory::create_input_request(login, passsword);

    BOOST_CHECK(!in_request_bin->has_register_request());
    BOOST_CHECK(in_request_bin->has_input_request());
    BOOST_CHECK(!in_request_bin->has_text_request());
    BOOST_CHECK(!in_request_bin->has_join_room_request());
}

BOOST_AUTO_TEST_CASE(auto_request_1) {
    std::string login = "vasiliy";
    std::string passsword = "a1s22g_1";

    auto in_request_bin = Protocol::MsgFactory::create_input_request(login, passsword);

    BOOST_CHECK(in_request_bin->input_request().login() == login);
    BOOST_CHECK(in_request_bin->input_request().password() == passsword);
}

BOOST_AUTO_TEST_CASE(auto_request_2) {
    std::string login = "vasilsdasafmdskndlskn dsmfkkdsnfkndsnfsdlnfkldsnflknsdfkndslkfniy";
    std::string passsword = "a1s22g_133289408324877gds8f7gds7gsd7g987dsfg87sd89g78ds7g7sdg987sd8g7987dsg";

    auto in_request_bin = Protocol::MsgFactory::create_input_request(login, passsword);

    BOOST_CHECK(in_request_bin->input_request().login() == login);
    BOOST_CHECK(in_request_bin->input_request().password() == passsword);
}
