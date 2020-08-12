#ifndef DATETIME_H
#define DATETIME_H

#include <boost/date_time/posix_time/posix_time.hpp>

struct DateTime
{
    DateTime();
    DateTime(const boost::posix_time::ptime& ptime);

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    uint8_t day;
    uint8_t month;
    uint8_t year; // since 2000

    std::string to_simple_date() const;

    std::string to_simple_time() const;

    static DateTime from_universal_to_local(DateTime dt);

    friend bool operator== (DateTime lhs, DateTime rhs);
    friend bool operator< (DateTime lhs, DateTime rhs);
};


#endif // DATETIME_H
