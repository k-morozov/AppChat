#include "logger.h"


void init_logger() {
    boost::log::add_file_log("sample.log");

    boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= boost::log::trivial::info
    );
}
