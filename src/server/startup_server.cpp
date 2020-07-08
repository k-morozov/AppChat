#include <server/server.h>
#include "logger.h"

int main() {
    init_logger();
    auto logger = LOGGER("Main");
    LOG4CPLUS_INFO(logger, "Server started. Version: 0.5");
    std::cout << "Server started. Version: 0.5" << std::endl;

    try {
        Server server;
        server.run();
    } catch (const std::exception & ex) {
        LOG4CPLUS_ERROR(logger, "exception " << ex.what());
        std::cout << ex.what() << std::endl;
    }
}
