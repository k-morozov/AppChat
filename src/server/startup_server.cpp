#include "server.h"
#include "log/logger.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace {
    /**
     * @brief set_parametrs - parser command line.
     * @param argc
     * @param argv
     * @param port
     * @return results for starts server
     */
    bool set_parametrs(int argc, char** argv,  unsigned short& port) {
        namespace po = boost::program_options;
        try {
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help, h", "produce help message")
                ("port, p", po::value<int>()->default_value(SERVER_DEFAULT_PORT), "set the port to connect the clients. int value")
            ;

            po::variables_map vm;
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);

            if (vm.count("help")) {
                BOOST_LOG_TRIVIAL(info) << desc;
                return true;
            }

            if (vm.count("port")) {
                port = static_cast<unsigned short>(vm["port"].as<int>());
                BOOST_LOG_TRIVIAL(info) << "Port was set to "
                     << port << ".";
            }
        }
        catch(std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << e.what();
            BOOST_LOG_TRIVIAL(error) << "use --help for more information.";
            return false;
        }
        catch(...) {
            BOOST_LOG_TRIVIAL(error) << "Exception of unknown type!";
            BOOST_LOG_TRIVIAL(error) << "use --help for more information.";
            return false;
        }

        BOOST_LOG_TRIVIAL(info) << "Finish parse command line";
        return true;
    }

}   // anonymous namespace

int main(int argc, char** argv) {
    init_logger();
    unsigned short port = SERVER_DEFAULT_PORT;

    if (set_parametrs(argc, argv, port)) {
        BOOST_LOG_TRIVIAL(info) << "starting server v.0.8, CXX=" << __cplusplus;
        try {
            database_ptr db_sqlite = std::make_shared<Database>();
            std::unique_ptr<Server> server = std::make_unique<Server>(port, db_sqlite);
            server->run();
        } catch (const std::exception & ex) {
            BOOST_LOG_TRIVIAL(fatal) << "Exception " << ex.what();
        }
    }

    BOOST_LOG_TRIVIAL(info) << "Server is shutting down now.";
}





