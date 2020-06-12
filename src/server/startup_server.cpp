// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "server.h"
#include "../log/logger.h"


int main() {
    init_logger();
    log4cplus::Logger  logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Main"));
    LOG4CPLUS_INFO(logger, "Server started. Version: 0.1");
    LOG4CPLUS_ERROR(logger, "error test");
    LOG4CPLUS_FATAL(logger, "fatal test");

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 7777);
    Server server(io_service, endpoint);

    io_service.run();
}
