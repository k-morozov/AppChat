#include <gtest/gtest.h>
#include <protocol/protocol.h>
#include <memory>

class response: public ::testing::Test {
protected:

    virtual void SetUp() override {

    }
    virtual void TearDown() override {

    }
    virtual void TestBody() override {

    }
};

TEST_F(response, registration_basic) {
    int32_t id = 12345;
    RegistrationResponse  response(id);

    EXPECT_EQ(response.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(response.get_type_data(), TypeCommand::RegistrationResponse);
    EXPECT_EQ(response.get_loginid(), id);
}

TEST_F(response, autorisation_basic) {
    uint32_t id = 1234567890;
    AutorisationResponse  response(id);

    EXPECT_EQ(response.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(response.get_type_data(), TypeCommand::AutorisationResponse);
    EXPECT_EQ(response.get_loginid(), id);
}

TEST_F(response, registration_basic_change_id) {
    uint32_t id = 12345;
    RegistrationResponse  response(id);
    id = 123;
    response.set_loginid(id);
    EXPECT_EQ(response.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(response.get_type_data(), TypeCommand::RegistrationResponse);
    EXPECT_EQ(response.get_loginid(), id);

    response.set_loginid();
    EXPECT_EQ(response.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(response.get_type_data(), TypeCommand::RegistrationResponse);
    EXPECT_EQ(response.get_loginid(), 0);
}

TEST_F(response, autorisation_basic_change_id) {
    uint32_t id = 1234567890;
    AutorisationResponse  response(id);
    id = 123;
    response.set_loginid(id);

    EXPECT_EQ(response.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(response.get_type_data(), TypeCommand::AutorisationResponse);
    EXPECT_EQ(response.get_loginid(), id);

    response.set_loginid();
    EXPECT_EQ(response.get_protocol_version(), PROTOCOL_VERS);
    EXPECT_EQ(response.get_type_data(), TypeCommand::AutorisationResponse);
    EXPECT_EQ(response.get_loginid(), 0);
}
