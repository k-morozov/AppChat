#include "logger.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

/**
 * @brief init_logger
 * @todo create log file
 */
void init_logger() {
//    logging::add_file_log
//    (
//        keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
//        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
//        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
//        keywords::format = "[%TimeStamp%]- <%Severity%>: %Message%"                                 /*< log record format >*/
//    );
//    boost::log::core::get()->set_filter
//        (
//            boost::log::trivial::severity >= boost::log::trivial::info
//        );
//    "%Y-%m-%d %H:%M:%S" [%ThreadID%]
    logging::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%]<%Severity%>: %Message%");

    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );

    logging::add_common_attributes();
}

