#include <string>

#include <boost/asio.hpp>
#include <QApplication>

#include "protocol/protocol.h"
#include "control/control.h"
#include "gui/mainwindow.h"

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>


int main(int argc, char** argv) {
    QApplication a(argc, argv);

    namespace po = boost::program_options;
    po::options_description desc("Client options");
    desc.add_options()
    ("ip, I", po::value<std::string>(), "ip-address")
    ("port, P", po::value<int32_t>(), "port")
    ;
    po::variables_map vm;
    try {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
    }
    catch(std::exception& ex) {
       std::cout << desc << std::endl;
    }
    Control control(vm);

    return a.exec();
}
