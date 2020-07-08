#include <string>

#include <boost/asio.hpp>
#include <QApplication>

#include <protocol.h>
#include <client/control.h>
#include <gui/mainwindow.h>


int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
    QApplication a(argc, argv);

    Control control;

    return a.exec();
}
