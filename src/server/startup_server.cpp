#include <server.h>
#include <logger.h>

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
    init_logger();
    auto logger = LOGGER("Main");
    LOG4CPLUS_INFO(logger, "Server started. Version: 0.5");

    try {
        Server server;
        server.run();
    } catch (const std::exception & ex) {
        LOG4CPLUS_ERROR(logger, "exception " << ex.what());
    }
}
