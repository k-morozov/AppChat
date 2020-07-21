#ifndef DATETIME_H
#define DATETIME_H

#include <boost/date_time/posix_time/posix_time_type.hpp>

struct DateTime
{
    DateTime() = default;
    DateTime(const boost::posix_time::ptime& ptime)
        : hours(static_cast<uint8_t>( ptime.time_of_day().hours() ))
        , minutes(static_cast<uint8_t>( ptime.time_of_day().minutes() ))
        , seconds(static_cast<uint8_t>( ptime.time_of_day().seconds() ))
        , day(static_cast<uint8_t>( ptime.date().day() ))
        , month(static_cast<uint8_t>( ptime.date().month() ))
        , year(static_cast<uint8_t>( ptime.date().year() - 2000 ))
    {}

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    uint8_t day;
    uint8_t month;
    uint8_t year; // since 2000
};

#endif // DATETIME_H
