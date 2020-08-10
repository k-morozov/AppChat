#include <string>

#include <boost/asio.hpp>
#include <QApplication>

#include "protocol/protocol.h"
#include "control/control.h"
#include "gui/mainwindow.h"


int main(int argc, char** argv) {
    QApplication a(argc, argv);
    Control control(argc, argv);

    return a.exec();
}
