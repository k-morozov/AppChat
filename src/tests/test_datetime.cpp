//#include <boost/test/unit_test.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include "datetime.h"

///**
// * @brief Unit tests for AppChat project.
// */
//BOOST_AUTO_TEST_SUITE(test_request)

//BOOST_AUTO_TEST_CASE(construction_by_default) {
//    DateTime dt;

//    BOOST_CHECK(dt.hours == 0);
//    BOOST_CHECK(dt.minutes == 0);
//    BOOST_CHECK(dt.seconds == 0);
//    BOOST_CHECK(dt.day == 0);
//    BOOST_CHECK(dt.month == 0);
//    BOOST_CHECK(dt.year == 0);
//}

//BOOST_AUTO_TEST_CASE(construction_from_ptime) {
//    using namespace boost::posix_time;

//    ptime pt = second_clock::universal_time();

//    DateTime dt(pt);

//    BOOST_CHECK(dt.hours == pt.time_of_day().hours());
//    BOOST_CHECK(dt.minutes == pt.time_of_day().minutes());
//    BOOST_CHECK(dt.seconds == pt.time_of_day().seconds());

//    BOOST_CHECK(dt.day == pt.date().day());
//    BOOST_CHECK(dt.month == pt.date().month());
//    BOOST_CHECK(dt.year + 2000 == pt.date().year());
//}

//BOOST_AUTO_TEST_CASE(simple_date_string) {
//    DateTime dt;
    
//    dt.day = 22;
//    dt.month = 7;
//    dt.year = 20;

//    BOOST_CHECK(dt.to_simple_date() == "22.07.2020");

//    dt.day = 0;
//    dt.month = 0;
//    dt.year = 0;

//    BOOST_CHECK(dt.to_simple_date() == "00.00.2000");

//    dt.day = 31;
//    dt.month = 12;
//    dt.year = 255;

//    BOOST_CHECK(dt.to_simple_date() == "31.12.2255");
//}

//BOOST_AUTO_TEST_CASE(simple_time_string) {
//    DateTime dt;

//    dt.hours = 16;
//    dt.minutes = 5;
//    dt.seconds = 50;

//    BOOST_CHECK(dt.to_simple_time() == "16:05:50");

//    dt.hours = 0;
//    dt.minutes = 0;
//    dt.seconds = 0;

//    BOOST_CHECK(dt.to_simple_time() == "00:00:00");

//    dt.hours = 23;
//    dt.minutes = 59;
//    dt.seconds = 59;

//    BOOST_CHECK(dt.to_simple_time() == "23:59:59");
//}

//BOOST_AUTO_TEST_CASE(operator_equal) {
//    DateTime lhs;

//    lhs.hours = 1;
//    lhs.minutes = 2;
//    lhs.seconds = 3;

//    lhs.day = 4;
//    lhs.month = 5;
//    lhs.year = 6;

//    DateTime rhs;

//    rhs.hours = 1;
//    rhs.minutes = 2;
//    rhs.seconds = 3;

//    rhs.day = 4;
//    rhs.month = 5;
//    rhs.year = 6;

//    BOOST_CHECK(lhs == rhs);

//    rhs.year = 7;

//    BOOST_CHECK(!(lhs == rhs));

//    rhs.year = 5;

//    BOOST_CHECK(!(lhs == rhs));
//}

//BOOST_AUTO_TEST_CASE(operator_less) {
//    DateTime lhs;

//    lhs.seconds = 1;
//    lhs.minutes = 2;
//    lhs.hours = 3;

//    lhs.day = 4;
//    lhs.month = 5;
//    lhs.year = 6;

//    DateTime rhs;

//    rhs.seconds = 1;
//    rhs.minutes = 2;
//    rhs.hours = 3;

//    rhs.day = 4;
//    rhs.month = 5;
//    rhs.year = 6;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.year = 7;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.year = 6;
//    lhs.month = 6;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.month = 5;
//    lhs.day = 5;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.day = 4;
//    lhs.hours = 4;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.hours = 3;
//    lhs.minutes = 3;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.minutes = 2;
//    lhs.seconds = 2;

//    BOOST_CHECK(!(lhs < rhs));

//    lhs.seconds = 0;

//    BOOST_CHECK(lhs < rhs);

//    lhs.seconds = 1;
//    lhs.minutes = 1;

//    BOOST_CHECK(lhs < rhs);

//    lhs.minutes = 2;
//    lhs.hours = 2;

//    BOOST_CHECK(lhs < rhs);

//    lhs.hours = 3;
//    lhs.day = 3;

//    BOOST_CHECK(lhs < rhs);

//    lhs.day = 4;
//    lhs.month = 4;

//    BOOST_CHECK(lhs < rhs);

//    lhs.month = 5;
//    lhs.year = 5;

//    BOOST_CHECK(lhs < rhs);
//}

//}
