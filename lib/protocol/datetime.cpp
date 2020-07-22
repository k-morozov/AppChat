#include "datetime.h"

#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/format.hpp>

DateTime::DateTime()
    : hours(0), minutes(0), seconds(0), day(0), month(0), year(0)
{ }

DateTime::DateTime(const boost::posix_time::ptime& ptime)
    : hours(static_cast<uint8_t>( ptime.time_of_day().hours() ))
    , minutes(static_cast<uint8_t>( ptime.time_of_day().minutes() ))
    , seconds(static_cast<uint8_t>( ptime.time_of_day().seconds() ))
    , day(static_cast<uint8_t>( ptime.date().day() ))
    , month(static_cast<uint8_t>( ptime.date().month() ))
    , year(static_cast<uint8_t>( ptime.date().year() - 2000 ))
{ }

std::string DateTime::to_simple_date() const
{
    return boost::str(boost::format("%1$#02d.%2$#02d.%3$d")
        % static_cast<unsigned int>(day)
        % static_cast<unsigned int>(month) % (year + 2000));
}

std::string DateTime::to_simple_time() const
{
    return boost::str(boost::format("%1$#02s:%2$#02s:%3$#02s")
        % static_cast<unsigned int>(hours)
        % static_cast<unsigned int>(minutes)
        % static_cast<unsigned int>(seconds));
}

DateTime DateTime::from_universal_to_local(DateTime dt)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    ptime pt(
        date(dt.year + 2000, dt.month, dt.day),
        time_duration(dt.hours, dt.minutes, dt.seconds)        
    );

    return DateTime(boost::date_time::c_local_adjustor<ptime>::utc_to_local(pt));
}

bool operator== (DateTime lhs, DateTime rhs)
{
    return lhs.hours == rhs.hours && lhs.minutes == rhs.minutes && lhs.seconds == rhs.seconds
        && lhs.day == rhs.day && lhs.month == rhs.month && lhs.year == rhs.year;
}

bool operator< (DateTime lhs, DateTime rhs)
{
    return lhs.year < rhs.year || lhs.month < rhs.month || lhs.day < rhs.day
        || lhs.hours < rhs.hours || lhs.minutes < rhs.minutes || lhs.seconds < rhs.seconds;
}
