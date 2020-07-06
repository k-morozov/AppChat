#include <string>

#include <boost/asio.hpp>
#include <QApplication>

#include <protocol/protocol.h>
#include <client/control.h>
#include <gui/mainwindow.h>


int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Control control(w);
    QObject::connect(&w, SIGNAL(send_input_data(const std::string&, const std::string&)),
                     &control, SLOT(autorisation(const std::string&, const std::string&)));
    QObject::connect(&w, SIGNAL(send_change_room(int)),
                     &control, SLOT(change_room(int)));

    return a.exec();
}
