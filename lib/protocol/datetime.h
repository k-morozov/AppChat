#ifndef DATETIME_H
#define DATETIME_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/format.hpp>

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
    { }

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    uint8_t day;
    uint8_t month;
    uint8_t year; // since 2000

    std::string to_simple_date() const
    {
        return boost::str(boost::format("%1$#02d.%2$#02d.%3$d")
            % static_cast<unsigned int>(day)
            % static_cast<unsigned int>(month) % (year + 2000));
    }

    std::string to_simple_time() const
    {
        return boost::str(boost::format("%1$#02s:%2$#02s:%3$#02s")
            % static_cast<unsigned int>(hours)
            % static_cast<unsigned int>(minutes)
            % static_cast<unsigned int>(seconds));
    }

    static auto from_universal_to_local(DateTime dt)
    {
        using namespace boost::posix_time;
        using namespace boost::gregorian;

        ptime pt(
            date(dt.year + 2000, dt.month, dt.day),
            time_duration(dt.hours, dt.minutes, dt.seconds)        
        );

        return DateTime(boost::date_time::c_local_adjustor<ptime>::utc_to_local(pt));
    }
};

#endif // DATETIME_H
