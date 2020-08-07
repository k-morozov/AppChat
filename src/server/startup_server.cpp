#include "server.h"
#include "log/logger.h"

int main(int argc, char** argv) {
    boost::log::core::get()->set_logging_enabled(true);
    BOOST_LOG_TRIVIAL(info) << "Server started. Version: 0.7";
    try {
        Server server((argc>1 ? std::stoi(argv[1]) : SERVER_DEFAULT_PORT));
        server.run();
    } catch (const std::exception & ex) {
        BOOST_LOG_TRIVIAL(info) << "Exception " << ex.what();
    }

    BOOST_LOG_TRIVIAL(info) << "Server is shutting down now with EXIT_SUCCESS";
}
