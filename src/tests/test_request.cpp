//#include <boost/test/unit_test.hpp>

//#include <protocol.h>
//#include <memory>

///**
// * @brief Unit tests for AppChat project.
// */
//BOOST_AUTO_TEST_SUITE(test_request)

//BOOST_AUTO_TEST_CASE(registration_basic) {
//    std::string login = "vasiliy";
//    std::string passsword = "a1s22g_1";

//    RegistrationRequest  request(login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::RegistrationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(registration_empty_login) {
//    std::string login = "";
//    std::string passsword = "a1s22g";
//    RegistrationRequest  request(login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::RegistrationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(registration_empty_password) {
//    std::string login = "vasiliy";
//    std::string passsword       = "";
//    RegistrationRequest  request(login.data(), passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::RegistrationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(registration_over_login) {
//    std::string fake_login = "vasiliyaaaaaaaaBBBB";
//    std::string login = "vasiliyaaaaaaaa";
//    std::string passsword = "a1s22g";
//    RegistrationRequest  request(fake_login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::RegistrationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(registration_over_password) {
//    std::string login = "vasiliy";
//    std::string fake_passsword  = "12345678901234567890123456789012345";
//    std::string passsword       = "1234567890123456789012345678901";
//    RegistrationRequest  request(login.data(), fake_passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::RegistrationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(autorisation_basic) {
//    std::string login = "vasiliy";
//    std::string passsword = "a1s22g_1";

//    AutorisationRequest  request(login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::AuthorisationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(autorisation_empty_login) {
//    std::string login = "";
//    std::string passsword = "a1s22g_1";

//    AutorisationRequest  request(login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::AuthorisationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(autorisation_empty_password) {
//    std::string login = "vasiliy";
//    std::string passsword = "";

//    AutorisationRequest  request(login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::AuthorisationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(autorisation_over_login) {
//    std::string fake_login      = "vasiliyaaaaaaaaBBBB";
//    std::string login           = "vasiliyaaaaaaaa";
//    std::string passsword = "a1s22g";
//    AutorisationRequest  request(fake_login.data(),passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::AuthorisationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(autorisation_over_password) {
//    std::string login = "vasiliy";
//    std::string fake_passsword  = "12345678901234567890123456789012345";
//    std::string passsword       = "1234567890123456789012345678901";
//    AutorisationRequest  request(login.data(), fake_passsword.data());

//    BOOST_CHECK(request.get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request.get_type_data() == TypeCommand::AuthorisationRequest);
//    BOOST_CHECK(request.get_login() == login);
//    BOOST_CHECK(request.get_password() == passsword);
//}

//BOOST_AUTO_TEST_CASE(autorisation_basic_ptr) {
//    std::string login = "vasiliy";
//    std::string password = "a1s22g_1";

//    autor_request_ptr request;
//    request = std::make_shared<AutorisationRequest>(login.data(), password.data());

//    BOOST_CHECK(request->get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request->get_type_data() == TypeCommand::AuthorisationRequest);
//    BOOST_CHECK(request->get_login() == login);
//    BOOST_CHECK(request->get_password() == password);
//}

//BOOST_AUTO_TEST_CASE(input_basic_empty_ptr) {
//    input_request_ptr request;
//    request = std::make_shared<AutorisationRequest>();

//    BOOST_CHECK(request->get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request->get_type_data() == TypeCommand::AuthorisationRequest);
//}

//BOOST_AUTO_TEST_CASE(autorisation_basic_empty_ptr) {
//    autor_request_ptr request;
//    request = std::make_shared<AutorisationRequest>();

//    BOOST_CHECK(request->get_protocol_version() == PROTOCOL_VERS);
//    BOOST_CHECK(request->get_type_data() == TypeCommand::AuthorisationRequest);
//}
//} // end boost::test
