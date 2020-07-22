#include <gtest/gtest.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "datetime.h"

class datetime : public ::testing::Test
{
protected:
    virtual void SetUp() override { }
    virtual void TearDown() override { }
};

TEST_F(datetime, construction_by_default)
{
    DateTime dt;

    EXPECT_EQ(dt.hours, 0);
    EXPECT_EQ(dt.minutes, 0);
    EXPECT_EQ(dt.seconds, 0);

    EXPECT_EQ(dt.day, 0);
    EXPECT_EQ(dt.month, 0);
    EXPECT_EQ(dt.year, 0);
}

TEST_F(datetime, construction_from_ptime)
{
    using namespace boost::posix_time;

    ptime pt = second_clock::universal_time();

    DateTime dt(pt);

    EXPECT_EQ(dt.hours, pt.time_of_day().hours());
    EXPECT_EQ(dt.minutes, pt.time_of_day().minutes());
    EXPECT_EQ(dt.seconds, pt.time_of_day().seconds());

    EXPECT_EQ(dt.day, pt.date().day());
    EXPECT_EQ(dt.month, pt.date().month());
    EXPECT_EQ(dt.year + 2000, pt.date().year());
}

TEST_F(datetime, simple_date_string)
{
    DateTime dt;
    
    dt.day = 22;
    dt.month = 7;
    dt.year = 20;

    EXPECT_EQ(dt.to_simple_date(), "22.07.2020");

    dt.day = 0;
    dt.month = 0;
    dt.year = 0;

    EXPECT_EQ(dt.to_simple_date(), "00.00.2000");

    dt.day = 31;
    dt.month = 12;
    dt.year = 255;

    EXPECT_EQ(dt.to_simple_date(), "31.12.2255");
}

TEST_F(datetime, simple_time_string)
{
    DateTime dt;

    dt.hours = 16;
    dt.minutes = 5;
    dt.seconds = 50;

    EXPECT_EQ(dt.to_simple_time(), "16:05:50");

    dt.hours = 0;
    dt.minutes = 0;
    dt.seconds = 0;

    EXPECT_EQ(dt.to_simple_time(), "00:00:00");

    dt.hours = 23;
    dt.minutes = 59;
    dt.seconds = 59;

    EXPECT_EQ(dt.to_simple_time(), "23:59:59");
}

TEST_F(datetime, operator_equal)
{
    DateTime lhs;

    lhs.hours = 1;
    lhs.minutes = 2;
    lhs.seconds = 3;

    lhs.day = 4;
    lhs.month = 5;
    lhs.year = 6;

    DateTime rhs;

    rhs.hours = 1;
    rhs.minutes = 2;
    rhs.seconds = 3;

    rhs.day = 4;
    rhs.month = 5;
    rhs.year = 6;

    EXPECT_EQ(lhs == rhs, true);

    rhs.year = 7;

    EXPECT_EQ(lhs == rhs, false);

    rhs.year = 5;

    EXPECT_EQ(lhs == rhs, false);
}

TEST_F(datetime, operator_less)
{
    DateTime lhs;

    lhs.seconds = 1;
    lhs.minutes = 2;
    lhs.hours = 3;

    lhs.day = 4;
    lhs.month = 5;
    lhs.year = 6;

    DateTime rhs;

    rhs.seconds = 1;
    rhs.minutes = 2;
    rhs.hours = 3;

    rhs.day = 4;
    rhs.month = 5;
    rhs.year = 6;

    EXPECT_EQ(lhs < rhs, false);

    lhs.year = 7;

    EXPECT_EQ(lhs < rhs, false);

    lhs.year = 6;
    lhs.month = 6;

    EXPECT_EQ(lhs < rhs, false);

    lhs.month = 5;
    lhs.day = 5;

    EXPECT_EQ(lhs < rhs, false);

    lhs.day = 4;
    lhs.hours = 4;

    EXPECT_EQ(lhs < rhs, false);

    lhs.hours = 3;
    lhs.minutes = 3;

    EXPECT_EQ(lhs < rhs, false);

    lhs.minutes = 2;
    lhs.seconds = 2;

    EXPECT_EQ(lhs < rhs, false);

    lhs.seconds = 0;

    EXPECT_EQ(lhs < rhs, true);

    lhs.seconds = 1;
    lhs.minutes = 1;

    EXPECT_EQ(lhs < rhs, true);

    lhs.minutes = 2;
    lhs.hours = 2;

    EXPECT_EQ(lhs < rhs, true);

    lhs.hours = 3;
    lhs.day = 3;

    EXPECT_EQ(lhs < rhs, true);

    lhs.day = 4;
    lhs.month = 4;

    EXPECT_EQ(lhs < rhs, true);

    lhs.month = 5;
    lhs.year = 5;

    EXPECT_EQ(lhs < rhs, true);
}

