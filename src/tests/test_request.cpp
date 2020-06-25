#include <gtest/gtest.h>
#include <protocol/protocol.h>

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
    EXPECT_EQ(request.get_type_command(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_empty_login) {
    std::string login = "";
    std::string passsword = "a1s22g";
    RegistrationRequest  request(login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_command(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_empty_password) {
    std::string login = "vasiliy";
    std::string passsword       = "";
    RegistrationRequest  request(login.data(), passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_command(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_over_login) {
    std::string fake_login = "vasiliyaaa";
    std::string login = "vasiliy";
    std::string passsword = "a1s22g";
    RegistrationRequest  request(fake_login.data(),passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_command(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}

TEST_F(request, registration_over_password) {
    std::string login = "vasiliy";
    std::string fake_passsword  = "12345678901234567890123456789012345";
    std::string passsword       = "1234567890123456789012345678901";
    RegistrationRequest  request(login.data(), fake_passsword.data());

    EXPECT_EQ(request.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(request.get_type_command(), static_cast<uint16_t>(TypeCommand::RegistrationRequest));
    EXPECT_EQ(request.get_login(), login);
    EXPECT_EQ(request.get_password(), passsword);
}


