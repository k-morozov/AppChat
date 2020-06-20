// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <server/server.h>
#include <log/logger.h>

int main() {
    init_logger();
    auto logger = LOGGER("Main");
    LOG4CPLUS_INFO(logger, "Server started. Version: 0.2");
    std::cout << "Server started. Version: 0.2" << std::endl;

    try {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 7777);
        Server server(io_service, endpoint);
        io_service.run();

    } catch (const std::exception & ex) {
        LOG4CPLUS_ERROR(logger, "exception " << ex.what());
        std::cout << ex.what() << std::endl;
    }
}
