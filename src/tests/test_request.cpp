#include <gtest/gtest.h>
#include <protocol/protocol.h>
#include <memory>

class request: public ::testing::Test {
protected:

    virtual void SetUp() override {

    }
    virtual void TearDown() override {

    }
    virtual void TestBody() override {

    }
};


TEST_F(request, registration_basic) {
    std::string login = "vasiliy";
    std::string passsword = "a1s22g_1";

    RegistrationRequest  request(login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::RegistrationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_empty_login) {
    std::string login = "";
    std::string passsword = "a1s22g";
    RegistrationRequest  request(login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::RegistrationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_empty_password) {
    std::string login = "vasiliy";
    std::string passsword       = "";
    RegistrationRequest  request(login.data(), passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::RegistrationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_over_login) {
    std::string fake_login = "vasiliyaaa";
    std::string login = "vasiliy";
    std::string passsword = "a1s22g";
    RegistrationRequest  request(fake_login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::RegistrationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_over_password) {
    std::string login = "vasiliy";
    std::string fake_passsword  = "12345678901234567890123456789012345";
    std::string passsword       = "1234567890123456789012345678901";
    RegistrationRequest  request(login.data(), fake_passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::RegistrationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

//TEST_F(request, registration_init) {
//    std::string login = "vasiliy";
//    std::string passsword       = "1234567890123456789012345678901";
//    RegistrationRequest  request(login.data(), passsword.data());

//    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
//    EXPECT_EQ(request.get_type_data(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
//    EXPECT_EQ(request.get_login(), login);
//    EXPECT_EQ(request.get_password(), passsword);

//    RegistrationRequest after_read_request;
//    after_read_request.init(request.get_data());

//    EXPECT_EQ(after_read_request.get_protocol_version(), PROTOCOL_VERS);
//    EXPECT_EQ(after_read_request.get_type_data(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
//    EXPECT_EQ(after_read_request.get_login(), login);
//    EXPECT_EQ(after_read_request.get_password(), passsword);
//}

TEST_F(request, autorisation_basic) {
    std::string login = "vasiliy";
    std::string passsword = "a1s22g_1";

    AutorisationRequest  request(login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::AuthorisationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, autorisation_empty_login) {
    std::string login = "";
    std::string passsword = "a1s22g_1";

    AutorisationRequest  request(login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::AuthorisationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, autorisation_empty_password) {
    std::string login = "vasiliy";
    std::string passsword = "";

    AutorisationRequest  request(login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::AuthorisationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, autorisation_over_login) {
    std::string fake_login = "vasiliyaaa";
    std::string login = "vasiliy";
    std::string passsword = "a1s22g";
    AutorisationRequest  request(fake_login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::AuthorisationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, autorisation_over_password) {
    std::string login = "vasiliy";
    std::string fake_passsword  = "12345678901234567890123456789012345";
    std::string passsword       = "1234567890123456789012345678901";
    AutorisationRequest  request(login.data(), fake_passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_data(), TypeCommand::AuthorisationRequest);
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

//TEST_F(request, autorisation_init) {
//    std::string login = "vasiliy";
//    std::string passsword       = "1234567890123456789012345678901";
//    AutorisationRequest  request(login.data(), passsword.data());

//    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
//    EXPECT_EQ(request.get_type_data(), static_cast<uint16_t>(TypeCommand::AuthorisationRequest));
//    EXPECT_EQ(request.get_login(), login);
//    EXPECT_EQ(request.get_password(), passsword);

//    AutorisationRequest after_read_request;
//    after_read_request.init(request.get_data());

//    EXPECT_EQ(after_read_request.get_protocol_version(), PROTOCOL_VERS);
//    EXPECT_EQ(after_read_request.get_type_data(), static_cast<uint16_t>(TypeCommand::AuthorisationRequest));
//    EXPECT_EQ(after_read_request.get_login(), login);
//    EXPECT_EQ(after_read_request.get_password(), passsword);
//}

TEST_F(request, autorisation_basic_ptr) {
    std::string login = "vasiliy";
    std::string password = "a1s22g_1";

    autor_req_ptr request;
    request = std::make_shared<AutorisationRequest>(login.data(), password.data());

    EXPECT_EQ(request->get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request->get_type_data(), TypeCommand::AuthorisationRequest);
    EXPECT_EQ(request->get_login(), login);
    EXPECT_EQ(request->get_password(), password);
}

TEST_F(request, input_basic_empty_ptr) {
    input_request_ptr request;
    request = std::make_shared<AutorisationRequest>();

    EXPECT_EQ(request->get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request->get_type_data(), TypeCommand::AuthorisationRequest);
}

TEST_F(request, autorisation_basic_empty_ptr) {
    autor_req_ptr request;
    request = std::make_shared<AutorisationRequest>();

    EXPECT_EQ(request->get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request->get_type_data(), TypeCommand::AuthorisationRequest);
}
