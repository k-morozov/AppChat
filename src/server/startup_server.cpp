#include "server.h"
#include "log/logger.h"


int main(int argc, char** argv) {
    init_logger();
    auto logger = LOGGER("Main");
    LOG4CPLUS_INFO(logger, "Server started. Version: 0.7");

    try {
        Server server((argc>1 ? std::stoi(argv[1]) : SERVER_DEFAULT_PORT));
        server.run();
    } catch (const std::exception & ex) {
        LOG4CPLUS_ERROR(logger, "exception " << ex.what());
    }
}
