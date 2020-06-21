#include <gtest/gtest.h>

class first_test: public ::testing::Test {
protected:

    virtual void SetUp() override {

    }
    virtual void TearDown() override {

    }
    virtual void TestBody() override {

    }
};


TEST_F(first_test, test1) {
    EXPECT_EQ(9, 9);
}

TEST_F(first_test, test2) {
    EXPECT_EQ(9, 9);
}
TEST_F(first_test, test3) {
    EXPECT_EQ(9, 9);
}
