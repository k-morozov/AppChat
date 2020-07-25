#include <server.h>
#include <logger.h>
#include <traced_exception.h>

#include <boost/stacktrace.hpp>

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
    init_logger();
    auto logger = LOGGER("Main");
    LOG4CPLUS_INFO(logger, "Server started. Version: 0.5");

    try {
        Server server;
        server.run();
    } catch (const util::traced& ex) {
        LOG4CPLUS_ERROR(logger, "Exception: " << ex.what());
        LOG4CPLUS_ERROR(logger, "Stacktrace:\n" << ex.trace);
    } catch (const std::exception & ex) {
        LOG4CPLUS_ERROR(logger, "exception " << ex.what());
    }
}
